# Progetto-API-24-25: MOVHEX – Hexagonal Map Pathfinding

Questo progetto è stato sviluppato per la prova finale di Algoritmi e Strutture Dati 2024–2025.
Il programma modella una mappa composta da celle esagonali e consente di calcolare percorsi a costo minimo tra due posizioni, tenendo conto di costi dinamici e rotte aeree opzionali.

***DESCRIZIONE DEL PROGETTO***

La mappa è rappresentata come una griglia esagonale rettangolare.
Ogni cella ha un costo di uscita via terra e può disporre di rotte aeree monodirezionali con costo associato.
Il programma riceve comandi da standard input e risponde su standard output.

Le funzionalità principali includono:
  - Inizializzazione della mappa,
  - Modifica dei costi su un’area tramite distanza esagonale,
  - Aggiunta/rimozione di rotte aeree,
  - Calcolo del costo minimo di percorrenza tra due celle.

Il calcolo dei percorsi minimi è realizzato tramite Dijkstra con min-heap, garantendo efficienza anche su mappe di grandi dimensioni.

***CONTENUTO DELLA REPOSITORY***

  - Codice sorgente in C
  - Specifica ufficiale del progetto
  - File di input utilizzati per test, debugging e verifica di casi limite

***PRESTAZIONI***

Tempo di compilazione ed esecuzione: ~12 secondi

Memoria occupata: ~30 MiB

---------------------------------------------------------------------------------------
# API Project 2024–2025: MOVHEX – Hexagonal Map Pathfinding

This project was developed for the final exam of Algorithms and Data Structures 2024–2025.
The program models a map composed of hexagonal cells and allows the computation of minimum-cost paths between two positions, taking into account dynamic costs and optional air routes.

***PROJECT DESCRIPTION***

The map is represented as a rectangular hexagonal grid.
Each cell has a ground exit cost and may have unidirectional air routes with an associated cost.
The program receives commands from standard input and produces output on standard output.

The main features include:
  - Map initialization,
  - Cost modification over an area using hexagonal distance,
  - Addition/removal of air routes,
  - Computation of the minimum traversal cost between two cells.

Minimum path computation is implemented using Dijkstra’s algorithm with a min-heap, ensuring efficiency even on large maps.

***REPOSITORY CONTENTS***

  - C source code
  - Official project specification
  - Input files used for testing, debugging, and edge-case verification

***PERFORMANCE***

Compilation and execution time: ~12 seconds

Memory usage: ~30 MiB
