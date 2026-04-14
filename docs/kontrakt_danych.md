
## Struktura tematu (topic)
Przyjęliśmy i przetestowaliśmy następujący format:
lab/<group_id>/<device_id>/<sensor>

Przykład, którego używaliśmy w testach:
lab/g03/esp32-ab12cd34/temperature

## Struktura JSON-a (Payload)

Zdefiniowaliśmy wymogi walidacyjne dla pól wymaganych:
- device_id (niepusty tekst)
- sensor (tekst)
- value (liczba, nie tekst)
- ts_ms (czas w milisekundach - dodatnia liczba całkowita)

Pola opcjonalne, które dodaliśmy do kontraktu:
- schema_version, group_id, unit, seq

## Przykłady użyte podczas testów

Poprawny JSON (przechodził walidację i był zapisywany):
{
"schema_version": 1,
"group_id": "g03",
"device_id": "esp32-ab12cd34",
"sensor": "temperature",
"value": 24.5,
"unit": "C",
"ts_ms": 1742030400000,
"seq": 15
}

Błędny JSON (został celowo odrzucony podczas testów, ponieważ brakowało pola czasu, a wartość była tekstem):
{
"device_id": "esp32-ab12cd34",
"sensor": "temperature",
"value": "24.5",
"unit": "C"
}