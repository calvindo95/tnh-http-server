#include <DBQ.h>

#include <sstream>

DBQ::DBQ(){
    // Instantiate Driver
    m_driver = sql::mariadb::get_driver_instance();

    // Configure connection url
    std::stringstream ss;
    ss << "jdbc:mariadb://" << config.GET_DB_IP() << ":" << config.GET_DB_PORT() << "/" << config.GET_DB_NAME();
    m_url = ss.str();

    // Configure connection properties
    sql::Properties props({
        {"user", config.GET_DB_USERNAME()},
        {"password", config.GET_DB_PASSWORD()},
        {"autoReconnect", "TRUE"}
    });

    m_properties = props;

    std::unique_ptr<sql::Connection> conn(m_driver->connect(m_url,m_properties));
    m_conn = std::move(conn);
}

DBQ::~DBQ(){
    m_conn->close();
}

int DBQ::get_device_id(std::string devname){
    std::shared_ptr<sql::PreparedStatement> pstatement(m_conn->prepareStatement("SELECT DeviceID FROM Device WHERE DevName=?"));

    try{
        pstatement->setString(1, devname);

        // executeQuery() returns sql::ResultSet object
        std::unique_ptr<sql::ResultSet> res(pstatement->executeQuery());

        while(res->next()){
            return std::stoi(res->getString("DeviceID").c_str());
        }
        return 0;
    }
    catch(sql::SQLException &e){
        m_logger.log(Logging::severity_level::critical, e.what(), "GENTRACE");
        return -1;
    }
}

void DBQ::insert_history(double temp, double humidity, int device_id, const std::string& datetime){
    std::shared_ptr<sql::PreparedStatement> stmt(m_conn->prepareStatement(
        "INSERT INTO History (Temperature, Humidity, DeviceID, CurrentDateTime) VALUES(?, ?, ?, ?)"
    ));
    try {
        stmt->setDouble(1, temp);
        stmt->setDouble(2, humidity);
        stmt->setInt(3, device_id);
        stmt->setString(4, datetime);
        stmt->executeUpdate();
    }
    catch (sql::SQLException& e) {
        m_logger.log(Logging::severity_level::critical, e.what(), "GENTRACE");
    }
}

void DBQ::insert_history(double temp, double humidity, std::string devicename, const std::string& datetime){
        std::shared_ptr<sql::PreparedStatement> stmt(m_conn->prepareStatement(
        "INSERT INTO History (Temperature, Humidity, DeviceID, CurrentDateTime) VALUES(?, ?, (SELECT DeviceID FROM Device WHERE DevName = ?), ?)"
    ));
    try {
        stmt->setDouble(1, temp);
        stmt->setDouble(2, humidity);
        stmt->setString(3, devicename);
        stmt->setString(4, datetime);
        stmt->executeUpdate();
    }
    catch (sql::SQLException& e) {
        m_logger.log(Logging::severity_level::critical, e.what(), "GENTRACE");
    }
}

void DBQ::insert_devname(std::string devname){
    std::shared_ptr<sql::PreparedStatement> stmnt(m_conn->prepareStatement("INSERT INTO Device(DevName) VALUES (?)"));

    try{
        stmnt->setString(1, devname);

        // executeUpdate() returns nothing
        stmnt->executeUpdate();
    }
    catch(sql::SQLException &e){
        m_logger.log(Logging::severity_level::critical, e.what(), "GENTRACE");
    }
}

void DBQ::get_last_device_entry(int deviceid, nlohmann::json &json){
    std::shared_ptr<sql::PreparedStatement> pstatement(m_conn->prepareStatement("SELECT CurrentDateTime, DeviceID FROM History WHERE DeviceID=? ORDER BY CurrentDateTime DESC LIMIT 1"));
    nlohmann::json j;

    try{
        std::string cdt;
        pstatement->setString(1, std::to_string(deviceid));

        // executeQuery() returns sql::ResultSet object
        std::unique_ptr<sql::ResultSet> res(pstatement->executeQuery());

        while(res->next()){
            if(std::stoi(res->getString("DeviceID").c_str()) == deviceid){
                cdt = res->getString("CurrentDateTime");
            }
        }

        // If cdt is empty, return empty json obj
        if(cdt.empty()){
            std::stringstream ss;
            ss << "DeviceID " << deviceid << " does not exist";

            j["error"] = ss.str();

            json = j;
            return;
        }

        j["DeviceID"] = deviceid;
        j["CurrentDateTime"] = cdt;

        json = j;
        return;
    }
    catch(sql::SQLException &e){
        m_logger.log(Logging::severity_level::critical, e.what(), "GENTRACE");
        
        j["error"] = e.what();

        json = j;
        return;
    }
}

std::string DBQ::list_device_count(){
    std::unique_ptr<sql::PreparedStatement> pstatement(
        m_conn->prepareStatement(
            "SELECT d.DeviceID, d.DevName, COUNT(h.HistoryID) AS HistoryCount FROM Device d LEFT JOIN History h ON h.DeviceID = d.DeviceID GROUP BY d.DeviceID, d.DevName ORDER BY d.DeviceID ASC"
        )
    );

    std::unique_ptr<sql::ResultSet> res(pstatement->executeQuery());

    std::stringstream ss;
    ss << std::left << std::setw(12)  << "DeviceID" << std::setw(18) << "Device Name" << std::setw(12) << "Count" << std::endl;

    while(res->next()){
        int deviceid = res->getInt(1);
        sql::SQLString devicename = res->getString(2);
        uint64_t count = res->getUInt64(3);
    
        ss << std::setw(12) << deviceid  << std::setw(18) << devicename << std::setw(12) << count << std::endl;
    }

    return ss.str();
}

nlohmann::json DBQ::list_device_count_json(){
    std::unique_ptr<sql::PreparedStatement> pstatement(
        m_conn->prepareStatement(
            "SELECT d.DeviceID, d.DevName, COUNT(h.HistoryID) AS HistoryCount FROM Device d LEFT JOIN History h ON h.DeviceID = d.DeviceID GROUP BY d.DeviceID, d.DevName ORDER BY d.DeviceID ASC"
        )
    );

    std::unique_ptr<sql::ResultSet> res(pstatement->executeQuery());
    nlohmann::json result = nlohmann::json::array();

    while(res->next()){
        result.push_back({
            {"DeviceID",   res->getInt(1)},
            {"DeviceName", res->getString(2).c_str()},
            {"Count",      res->getUInt64(3)}
        });
    }

    return result;
}