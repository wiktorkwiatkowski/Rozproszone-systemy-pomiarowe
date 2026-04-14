# Raport: Ingestor MQTT i baza PostgreSQL

## Model danych (PostgreSQL)
Utworzyliśmy w bazie tabelę o nazwie `measurements`. Została ona zaprojektowana tak, aby przechowywać wszystkie odebrane pomiary. Zawiera kolumny: id, group_id, device_id, sensor, value, unit, ts_ms, seq, topic oraz received_at (generowane automatycznie przez bazę).

## Logika i działanie Ingestora
Napisaliśmy skrypt oparty o biblioteki paho.mqtt i psycopg2, w którym:
- Ustanowiono połączenie z brokerem MQTT.
- Włączono subskrypcję szerokiego tematu `lab/+/+/+`.
- Zaimplementowano funkcję walidującą (skrypt pomyślnie odrzucał wiadomości z brakiem wymaganych pól takich jak device_id, sensor, value i ts_ms).
- Połączono się z bazą PostgreSQL i zaimplementowano zapytanie INSERT, które wrzucało zweryfikowane dane do bazy.
- Całość osadzono w środowisku Docker Compose jako osobną usługę.

## Przeprowadzone testy (End-to-End)
Przetestowaliśmy cały przepływ danych przy użyciu narzędzia MQTT Explorer.
Wysłaliśmy poprawny payload JSON na temat `lab/g03/esp32-test/temperature`. Następnie, korzystając z konsoli PostgreSQL (zapytanie SELECT * FROM measurements), zweryfikowaliśmy, że dane pomyślnie przeszły od nadawcy, przez brokera, zostały zwalidowane przez ingestor i finalnie zapisały się w tabeli. Sprawdziliśmy również logi ingestora upewniając się, że ignoruje on wiadomości niespełniające kontraktu.