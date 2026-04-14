import json
import paho.mqtt.client as mqtt
import db as db
MQTT_HOST = "broker"
MQTT_PORT = 1883
MQTT_TOPIC = "lab/+/+/+"
DB_HOST = "database"
DB_NAME = "abcd_db"
DB_USER = "admin"
DB_PASSWORD = "admin_pass1234"


def save_measurement(topic, data):

    conn = db.get_db_connection()
    cur = conn.cursor()
    cur.execute("""
        INSERT INTO measurements
        (group_id, device_id, sensor, value, unit, ts_ms, seq, topic)
        VALUES (%s, %s, %s, %s, %s, %s, %s, %s)
    """, (
        data.get("group_id"),
        data["device_id"],
        data["sensor"],
        data["value"],
        data.get("unit"),
        data["ts_ms"],
        data.get("seq"),
        topic
    ))
    conn.commit()
    cur.close()
    conn.close()


def is_valid(data):
    device_id = data.get("device_id")
    if not isinstance(device_id, str) or len(device_id.strip()) == 0:
        return False

    sensor = data.get("sensor")
    if not isinstance(sensor, str):
        return False

    value = data.get("value")
    if not isinstance(value, (int, float)) or isinstance(value, bool):
        return False

    ts_ms = data.get("ts_ms")
    if not isinstance(ts_ms, int) or isinstance(ts_ms, bool) or ts_ms <= 0:
        return False

    if "unit" in data:
        if not isinstance(data["unit"], str):
            return False

    if "seq" in data:
        seq = data["seq"]
        if not isinstance(seq, int) or isinstance(seq, bool) or seq < 0:
            return False

    return True


def on_connect(client, userdata, flags, rc):
    print("Connected with result code", rc)
    client.subscribe(MQTT_TOPIC)


def on_message(client, userdata, msg):
    try:
        payload = msg.payload.decode("utf-8")
        data = json.loads(payload)
        if not is_valid(data):
            print("Invalid payload:", data)
            return

        save_measurement(msg.topic, data)
        print("Saved message from topic:", msg.topic)
    except json.JSONDecodeError:
        print("Error: Payload is not a valid JSON.")

    except Exception as e:
        print("Error:", e)


client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(MQTT_HOST, MQTT_PORT, 60)
client.loop_forever()
