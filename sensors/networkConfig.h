#define WIFI_SSID    "" // Config
#define WIFI_PASS    "" // Config
#define MQTT_BROKER  "" // Config
#define MQTT_USER    "" // Config
#define MQTT_PASS    "" // Config


#define TOPIC_PREFIX ""  // Config
#define TOPIC_DATA_PREFIX TOPIC_PREFIX "/sensors"

#define TOPIC_TEMPERATURE TOPIC_DATA_PREFIX "/temperature"
#define TOPIC_HUMIDITY TOPIC_DATA_PREFIX "/humidity"
#define TOPIC_AIR TOPIC_DATA_PREFIX "/air_quality"
#define TOPIC_UV TOPIC_DATA_PREFIX "/uv"
#define TOPIC_LIGHT TOPIC_DATA_PREFIX "/light"
#define TOPIC_JSON TOPIC_DATA_PREFIX "/json"
#define TOPIC_AI TOPIC_PREFIX "/ai/auto_respond"
#define TOPIC_DEBUG TOPIC_PREFIX "/debug/data"
