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
        {"autoReconnect", "FALSE"}
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

//"INSERT INTO History (Temperature, Humidity,DeviceID, CurrentDateTime) VALUES(" << j["Temperature"] << "," << j["Humidity"] << ","<< j["DeviceID"] << "," << j["CurrentDateTime"] << ");";
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