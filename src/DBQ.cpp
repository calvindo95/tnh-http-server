#include <DBQ.h>

#include <iomanip>
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

        j["error"] = "Database error";

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

void DBQ::get_last_device_data(int deviceid, nlohmann::json& json){
    std::unique_ptr<sql::PreparedStatement> stmt(
        m_conn->prepareStatement(
            "SELECT JSON_OBJECT('CurrentDateTime', `CurrentDateTime`, 'DeviceID', `DeviceID`, 'Temperature', `Temperature`, 'Humidity', `Humidity`) FROM History WHERE DeviceID = ? ORDER BY CurrentDateTime DESC LIMIT 1"
        )
    );

    try{
        stmt->setInt(1, deviceid);
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());

        if(res->next()){
            json = nlohmann::json::parse(res->getString(1).c_str());
        }
        else{
            json = {{"error", "No entry found for DeviceID " + std::to_string(deviceid)}};
        }
    }
    catch(sql::SQLException& e){
        m_logger.log(Logging::severity_level::critical, e.what(), "GENTRACE");
        json = {{"error", "Database error"}};
    }
}

nlohmann::json DBQ::get_device_history(int deviceid, const std::string& start, const std::string& end, int bucket_minutes){
    const int bucket = bucket_minutes < 1 ? 1 : bucket_minutes;

    std::unique_ptr<sql::PreparedStatement> stmt(m_conn->prepareStatement(
        "SELECT "
        "  FROM_UNIXTIME(FLOOR(UNIX_TIMESTAMP(CurrentDateTime) / (? * 60)) * (? * 60)) AS CurrentDateTime, "
        "  AVG(Temperature) AS Temperature, "
        "  AVG(Humidity) AS Humidity "
        "FROM History "
        "WHERE DeviceID = ? AND CurrentDateTime BETWEEN ? AND ? "
        "GROUP BY FLOOR(UNIX_TIMESTAMP(CurrentDateTime) / (? * 60)) "
        "ORDER BY CurrentDateTime ASC"
    ));

    nlohmann::json result = nlohmann::json::array();

    try {
        stmt->setInt(1, bucket);
        stmt->setInt(2, bucket);
        stmt->setInt(3, deviceid);
        stmt->setString(4, start);
        stmt->setString(5, end);
        stmt->setInt(6, bucket);

        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());
        while (res->next()) {
            result.push_back({
                {"CurrentDateTime", res->getString("CurrentDateTime").c_str()},
                {"Temperature",     res->getDouble("Temperature")},
                {"Humidity",        res->getDouble("Humidity")}
            });
        }
    }
    catch (sql::SQLException& e) {
        m_logger.log(Logging::severity_level::critical, e.what(), "GENTRACE");
    }

    return result;
}

int DBQ::get_user(const std::string& username, std::string& out_password_hash){
    std::shared_ptr<sql::PreparedStatement> stmt(m_conn->prepareStatement(
        "SELECT user_id, password_hash FROM Users WHERE username = ?"
    ));

    try{
        stmt->setString(1, username);
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());

        if(res->next()){
            out_password_hash = res->getString("password_hash").c_str();
            return res->getInt("user_id");
        }
        return 0;
    }
    catch(sql::SQLException& e){
        m_logger.log(Logging::severity_level::critical, e.what(), "AUTH");
        return -1;
    }
}

bool DBQ::create_session(int user_id, const std::string& session_id){
    std::shared_ptr<sql::PreparedStatement> stmt(m_conn->prepareStatement(
        "INSERT INTO Session_Store (session_id, user_id) VALUES (?, ?)"
    ));

    try{
        stmt->setString(1, session_id);
        stmt->setInt(2, user_id);
        stmt->executeUpdate();
        return true;
    }
    catch(sql::SQLException& e){
        m_logger.log(Logging::severity_level::critical, e.what(), "AUTH");
        return false;
    }
}

int DBQ::create_user(const std::string& username, const std::string& password_hash){
    std::shared_ptr<sql::PreparedStatement> stmt(m_conn->prepareStatement(
        "INSERT INTO Users (username, password_hash) VALUES (?, ?)"
    ));

    try{
        stmt->setString(1, username);
        stmt->setString(2, password_hash);
        stmt->executeUpdate();

        std::unique_ptr<sql::Statement> id_stmt(m_conn->createStatement());
        std::unique_ptr<sql::ResultSet> res(id_stmt->executeQuery("SELECT LAST_INSERT_ID()"));
        if(res->next()) return res->getInt(1);
        return -1;
    }
    catch(sql::SQLException& e){
        // 1062 = ER_DUP_ENTRY (duplicate username)
        if(e.getErrorCode() == 1062) return 0;
        m_logger.log(Logging::severity_level::critical, e.what(), "AUTH");
        return -1;
    }
}

void DBQ::delete_expired_sessions(){
    std::shared_ptr<sql::PreparedStatement> stmt(m_conn->prepareStatement(
        "DELETE FROM Session_Store WHERE start_time < DATE_SUB(NOW(), INTERVAL ? MINUTE)"
    ));

    try{
        stmt->setInt(1, config.GET_SESSION_TIMEOUT());
        stmt->executeUpdate();

        int deleted = stmt->getUpdateCount();
        if(deleted > 0)
            m_logger.log(Logging::severity_level::info,
                "Deleted " + std::to_string(deleted) + " expired session(s)", "AUTH");
    }
    catch(sql::SQLException& e){
        m_logger.log(Logging::severity_level::critical, e.what(), "AUTH");
    }
}

int DBQ::delete_session(const std::string& session_id){
    std::shared_ptr<sql::PreparedStatement> stmt(m_conn->prepareStatement(
        "DELETE FROM Session_Store WHERE session_id = ?"
    ));

    try{
        stmt->setString(1, session_id);
        stmt->executeUpdate();
        if(stmt->getUpdateCount() > 0)
            return 1;
        m_logger.log(Logging::severity_level::info, "Logout successfully deleted session_id: " + session_id, "AUTH");
        return 0;
    }
    catch(sql::SQLException& e){
        m_logger.log(Logging::severity_level::critical, e.what(), "AUTH");
        return -1;
    }
}

int DBQ::validate_session(const std::string& session_id){
    std::shared_ptr<sql::PreparedStatement> stmt(m_conn->prepareStatement(
        "SELECT user_id FROM Session_Store "
        "WHERE session_id = ? AND start_time >= DATE_SUB(NOW(), INTERVAL ? MINUTE)"
    ));

    try{
        stmt->setString(1, session_id);
        stmt->setInt(2, config.GET_SESSION_TIMEOUT());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());

        if(res->next()) return res->getInt("user_id");
        return 0;
    }
    catch(sql::SQLException& e){
        m_logger.log(Logging::severity_level::critical, e.what(), "AUTH");
        return -1;
    }
}
