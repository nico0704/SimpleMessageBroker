# SimpleMessageBroker

Simple Message Broker

Author: Nico Schroeder

Beschreibung und grundsätzliche Funktionsweise:

Der Simple Message Broker ist eine einfache Implementierung zum Vermitteln von 
Nachrichten.
Es gibt drei grundlegende Programme / Rollen im Simple Message Broker:
Publisher, Subscriber und Broker.
Ein Subscriber kann Topics beim Broker abonnieren. Der Publisher sendet Nachrichten zu 
Topics an den Broker, welcher dann diese Nachrichten zu den Subscribern weiterleitet, die 
dieses bestimmte Topic abonniert haben. 
Grundsätzlich kann jedes Topic abonniert und Nachrichten zu jedem Topic veröffentlicht 
werden. Es gibt keine vorgefertigte Liste an Topics im Broker.
Der Broker dient lediglich als Vermittler der Nachrichten, er speichert die Adressen der 
Subscriber und deren jeweiligen abonnierten Topics. Wenn eine Nachricht vom Publisher 
ankommt, leitet er diese an die Subscriber weiter, welche das Topic abonniert haben. Die 
Nachricht wird nicht vom Broker gespeichert. 
Publisher und Subscriber kennen sich nicht, die Kommunikation läuft ausschließlich über den 
Broker. Das verwendete Protokoll ist UDP. Publisher und Subscriber sind Clients und der 
Broker ist ein Server.

_______________________________________________________________________________________________

Implementierung smbpublish.c: 
Aufruf: ./smbpublish.c broker topic message

Zunächst wird gecheckt, ob die korrekte Anzahl an Argumenten über die Kommandozeile 
übergeben wurden.Die Message muss entweder ein Wort sein oder in Anführungszeichen 
übergeben werden, falls zu wenig oderzu viele Argumente übergeben wurden, bricht das 
Programm ab und es wird ein Hinweis zum korrekten Aufruf auf der Kommandozeile 
ausgegeben. 
War der Aufruf jedoch korrekt, wird dann eine 'broker_message' gebaut. Das ist ein String, 
der zusammengesetzt ist aus Kennzeichnung|Topic|Message. Kennzeichnung ist ein 'P', um 
dem Broker später zu signalisieren, dass man ein Publisher ist. Danach folgen das Topic und 
die Message aus der Kommandozeile, jeweils getrennt von einem vorher festgelegten und in 
der gesamtenArchitektur identischem Separator, welcher in meinem Programm der logische 
OR Operator ist ('|').
Dann wird dieser String mit dem UDP Protokoll an den Broker gesendet. Anschließend wird 
noch die Bestätigung des Brokers gelesen und auf die Kommandozeile ausgegeben. Danach 
endet dann das Programm.

_______________________________________________________________________________________________

Implementierung smbsubscribe.c:
Aufruf: ./smbsubscribe.c broker topic

Zunächst wird wieder überprüft, ob der Aufruf korrekt war. Falls der Aufruf passt, wird 
wieder eine 'broker_message' nach dem gleichen Schema wie im Publisher gebaut. Einziger 
Unterschied ist diesmal, dass die Kennzeichnung ein 'S' ist (für Subscriber) und 
logischerweise nur das Topic und keine Message verwendet. Diese „broker_message“wird 
dann an den Broker gesendet. Danach springt das Programm in eine Endlosschleife und 
wartet auf eingehende Nachrichten vom Broker für das abonnierte Topic und gibt diese ggf. 
dann auf der Kommandozeile aus. Um alle Topics zu abonnieren, kann ein Subscriber als 
Topic das '#' definieren. 

_______________________________________________________________________________________________

Implementierung smbbroker.c:
Aufruf: ./smbbroker.c

Der Broker ist der eigentliche Vermittler der Nachrichten. Er wartet in einer Endlosschleife 
auf eingehende Nachrichten von Publishern oder Subscribern. Wenn er eine Nachricht 
erhalten hat, wird zunächst mithilfe des ersten Zeichens der Nachricht überprüft, ob die 
eingehende Nachricht von einem Publisher (P) oder einem Server (S) kommt. 

Fall 1 (S): Die Nachricht kam von einem Subscriber, der ein Topic abonnieren will. Die 
Subscriberadresse und das jeweilige Topic werden im Broker gespeichert. Grundlage für die 
Speicherung dieser Daten ist eine single Linked List.

Ein neuer Subscriber wird immer als Head in die Liste eingefügt. 

Es gibt drei Funktionen, die für die Linked List hier definiert sind: 

createNewNode → um einen neuen Knoten zu erstellen
insertAtHead → um den Knoten in die Liste einzufügen (als neuen Head)
printListIterativeFromTop → Liste auf der Konsole ausgeben (wird im Code nicht verwendet, 
ist aber hilfreich für Debugging-Zwecke)

Fall 2 (P): Die Nachricht kam von einem Publisher. In diesem Fall wird die Nachricht unter 
Verwendung des Separatorzeichens dekonstruiert und in den Variablen topic und message
gespeichert. Dann wird die Linked List traversiert und geprüft, ob das gerade behandelte 
Topic identisch mit dem Topic des aktuellen Knotens in der Linked List ist. Wenn das der Fall 
ist, wird dann die Message (und zur Übersichtlichkeit auch das Topic) an den jeweiligen 
Subscriber weitergeleitet. Die Nachricht verfällt, wenn es kein übereinstimmendes Topic 
gab.

__________________________________________________________________________________________________

Noch nicht implementierte Funktionen aber denkbare Verbesserungen des SMB:
Um die Linked List sauber zu halten, könnte man die Signals bei Beenden der Subscriber Programme abfangen 
und dann eine Nachricht an den Broker schicken, der dann den 
jeweiligen Subscriber aus der Linked List löscht, damit diese nicht zu lang wird. 
