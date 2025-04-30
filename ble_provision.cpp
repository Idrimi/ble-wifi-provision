#include <sdbus-c++/sdbus-c++.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <map>
#include <cstdlib>

static constexpr auto BLUEZ_SERVICE        = "org.bluez";
static constexpr auto ADAPTER_PATH         = "/org/bluez/hci0";
static constexpr auto PROPERTIES_IFACE     = "org.freedesktop.DBus.Properties";
static constexpr auto AGENT_MANAGER_IFACE  = "org.bluez.AgentManager1";
static constexpr auto AGENT_IFACE          = "org.bluez.Agent1";
static constexpr auto GATT_MANAGER_IFACE   = "org.bluez.GattManager1";
static constexpr auto ADVERT_MGR_IFACE     = "org.bluez.LEAdvertisingManager1";
static constexpr auto GATT_SERVICE_IFACE   = "org.bluez.GattService1";
static constexpr auto GATT_CHAR_IFACE      = "org.bluez.GattCharacteristic1";
static constexpr auto ADVERT_IFACE         = "org.bluez.LEAdvertisement1";

static constexpr auto SERVICE_PATH         = "/example/service0";
static constexpr auto CHAR1_PATH           = "/example/service0/char1";
static constexpr auto CHAR2_PATH           = "/example/service0/char2";
static constexpr auto ADV_PATH             = "/example/advertisement0";

static const std::string SERVICE_UUID      = "12345678-1234-5678-1234-56789abcdef0";
static const std::string CHAR1_UUID        = SERVICE_UUID.substr(0, SERVICE_UUID.size()-1) + "1";
static const std::string CHAR2_UUID        = SERVICE_UUID.substr(0, SERVICE_UUID.size()-1) + "2";

int main()
{
    std::string ssid, psk;

    // Systembus + EventLoop
    auto connection = sdbus::createSystemBusConnection();
    connection->enterEventLoopAsync();

    // 1) Agent1 registrieren
    auto agentMgr = sdbus::createProxy(*connection, BLUEZ_SERVICE, "/");
    auto agentObj = sdbus::createObject(*connection, "/example/agent");
    agentObj->registerMethod("RequestPinCode").onInterface(AGENT_IFACE)
        .implementedAs([](sdbus::ObjectPath){ return std::string(); });
    agentObj->registerMethod("RequestPasskey").onInterface(AGENT_IFACE)
        .implementedAs([](sdbus::ObjectPath){ return uint32_t(0); });
    agentObj->registerMethod("AuthorizeService").onInterface(AGENT_IFACE)
        .implementedAs([](sdbus::ObjectPath, std::string){});
    agentObj->finishRegistration();
    agentMgr->callMethod("RegisterAgent").onInterface(AGENT_MANAGER_IFACE)
        .withArguments(sdbus::ObjectPath{"/example/agent"}, std::string("NoInputNoOutput"))
        .dontExpectReply();
    agentMgr->callMethod("RequestDefaultAgent").onInterface(AGENT_MANAGER_IFACE)
        .withArguments(sdbus::ObjectPath{"/example/agent"})
        .dontExpectReply();

    // 2) Adapter Pairable + Discoverable
    auto adapterProps = sdbus::createProxy(*connection, BLUEZ_SERVICE, ADAPTER_PATH);
    adapterProps->callMethod("Set").onInterface(PROPERTIES_IFACE)
        .withArguments(std::string("org.bluez.Adapter1"), std::string("Pairable"), sdbus::Variant(true))
        .dontExpectReply();
    adapterProps->callMethod("Set").onInterface(PROPERTIES_IFACE)
        .withArguments(std::string("org.bluez.Adapter1"), std::string("Discoverable"), sdbus::Variant(true))
        .dontExpectReply();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // 3) GATT Service & Characteristics
    auto svc = sdbus::createObject(*connection, SERVICE_PATH);
    svc->registerProperty("UUID").onInterface(GATT_SERVICE_IFACE).withGetter([]{ return SERVICE_UUID; });
    svc->registerProperty("Primary").onInterface(GATT_SERVICE_IFACE).withGetter([]{ return true; });
    svc->registerProperty("Characteristics").onInterface(GATT_SERVICE_IFACE)
        .withGetter([]{ return std::vector<sdbus::ObjectPath>{{CHAR1_PATH},{CHAR2_PATH}}; });
    svc->finishRegistration();

    auto c1 = sdbus::createObject(*connection, CHAR1_PATH);
    c1->registerProperty("UUID").onInterface(GATT_CHAR_IFACE).withGetter([]{ return CHAR1_UUID; });
    c1->registerProperty("Service").onInterface(GATT_CHAR_IFACE).withGetter([]{ return sdbus::ObjectPath{SERVICE_PATH}; });
    c1->registerProperty("Flags").onInterface(GATT_CHAR_IFACE)
        .withGetter([]{ return std::vector<std::string>{"write","write-without-response"}; });
    c1->registerMethod("WriteValue").onInterface(GATT_CHAR_IFACE)
        .implementedAs([&](const std::vector<uint8_t>& v, const std::map<std::string,sdbus::Variant>&){
            ssid = std::string(v.begin(), v.end());
            std::cout<<"[GATT] SSID: "<<ssid<<std::endl;
        });
    c1->finishRegistration();

    auto c2 = sdbus::createObject(*connection, CHAR2_PATH);
    c2->registerProperty("UUID").onInterface(GATT_CHAR_IFACE).withGetter([]{ return CHAR2_UUID; });
    c2->registerProperty("Service").onInterface(GATT_CHAR_IFACE).withGetter([]{ return sdbus::ObjectPath{SERVICE_PATH}; });
    c2->registerProperty("Flags").onInterface(GATT_CHAR_IFACE)
        .withGetter([]{ return std::vector<std::string>{"write","write-without-response"}; });
    c2->registerMethod("WriteValue").onInterface(GATT_CHAR_IFACE)
        .implementedAs([&](const std::vector<uint8_t>& v, const std::map<std::string,sdbus::Variant>&){
            psk = std::string(v.begin(), v.end());
            std::cout<<"[GATT] PSK: "<<psk<<std::endl;
        });
    c2->finishRegistration();

    // 4) Register Application
    auto gattMgr = sdbus::createProxy(*connection, BLUEZ_SERVICE, ADAPTER_PATH);
    gattMgr->callMethod("RegisterApplication").onInterface(GATT_MANAGER_IFACE)
        .withArguments(sdbus::ObjectPath{SERVICE_PATH}, std::map<std::string,sdbus::Variant>{})
        .dontExpectReply();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // 5) Advertisement
    auto advMgr = sdbus::createProxy(*connection, BLUEZ_SERVICE, ADAPTER_PATH);
    auto adv = sdbus::createObject(*connection, ADV_PATH);
    adv->registerProperty("Type").onInterface(ADVERT_IFACE).withGetter([]{ return std::string("peripheral"); });
    adv->registerProperty("ServiceUUIDs").onInterface(ADVERT_IFACE).withGetter([]{ return std::vector<std::string>{SERVICE_UUID}; });
    adv->registerProperty("LocalName").onInterface(ADVERT_IFACE).withGetter([]{ return std::string("Pi-Setup"); });
    adv->registerProperty("Includes").onInterface(ADVERT_IFACE).withGetter([]{ return std::vector<std::string>{"tx-power"}; });
    adv->registerMethod("Release").onInterface(ADVERT_IFACE).implementedAs([]{ std::cout<<"Adv released"<<std::endl; });
    adv->finishRegistration();
    advMgr->callMethod("RegisterAdvertisement").onInterface(ADVERT_MGR_IFACE)
        .withArguments(sdbus::ObjectPath{ADV_PATH}, std::map<std::string,sdbus::Variant>{})
        .dontExpectReply();

    std::cout<<"🟢 Advertising as Pi-Setup (pairable)... waiting credentials"<<std::endl;

    // 6) Wait & connect
    while(ssid.empty()||psk.empty()) std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::string cmd="nmcli device wifi connect ""+ssid+"" password ""+psk+""";
    std::cout<<"🔌 "<<cmd<<std::endl; std::system(cmd.c_str());
    advMgr->callMethod("UnregisterAdvertisement").onInterface(ADVERT_MGR_IFACE)
        .withArguments(sdbus::ObjectPath{ADV_PATH}).dontExpectReply();
    std::cout<<"✅ Done"<<std::endl;
    return 0;
}
