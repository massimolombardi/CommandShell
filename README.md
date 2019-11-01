@abstract
Questa classe implementa una shell che fornisce una lista di comandi personalizzati, implementati
con funzioni scritte secondo la propria necessità.
 *
La classe utilizza due strutture dati interne, una stl map<string, string> per memorizzare i parametri
di configurazione ed una stl map<string, CMD_STATUS(*)(char* const*)> per implementare
la lista di metodi da invocare per i comandi.
 *
Tutte le funzioni di callback devono avere il tipo di ritorno CMD_STATUS, definito all'interno di questa classe,
che nello specifico associa il valore CMD_OK ad un comando completato con successo ed un valore CMD_ERROR ad un
comando completato con esito negativo.
 *
E' possibile definire comandi che vengono eseguiti in background, impostando a TRUE il flag apposito del metodo
addCommand. Inoltre, se impostato sul file, è possibile eseguire tutti i comandi presenti nell'ambiente Linux sul
quale viene eseguita la shell.
 *
Il file di configurazione deve essere un file di testo avente la seguente struttura:
 - deve contenere delle coppie <nome, valore>
 - ognuna di queste coppie deve essere scritta nel file di testo nel modo seguente:
     - <bol>[<delimiter>]<nome><delimiter>[<delimiter>]<valore><eol>, dove:
     - <bol> rappresenta l'inizio riga (beginning of line)
     - <nome> deve essere una stringa alfanumerica che non inizia per '#' e non contiene spazi
     - <valore> deve essere una stringa alfanumerica non contenente spazi
     - <delimiter> puo' essere uno dei seguenti caratteri: ' ' oppure '\t'
     - <eol> rappresenta la fine della riga (end of line)
 - Ogni riga che inizia con il carattere '#' viene trascurata (in tal modo si possono inserire commenti)
 - Ogni riga vuota viene ignorata
 *
Il file di help viene letto all'avvio. E' possibile compilarlo tenendo conto delle seguenti regole:
             il testo racchiuso tra $testo$ viene evidenziato con SHELL_COMMAND_COLOR mentre il testo racchiuso
             tra £testo£ viene evidenziato con SHELL_COMMAND_ARG_COLOR.
             Il file viene letto e stampato interamente, saltando esclusivamente le linee che iniziano con #.
 *
@endabstract
 *
@author Massimo Lombardi
@version 0.2
@since 09/07/2019
 *
@bug L'output su standard output dei comandi personalizzati che vengono eseguiti in background disturba l'esecuzione della shell e viene interpretato come un comando
@bug L'input dei comandi da pipe non funziona correttamente.
@bug La redirezione dell'output dei comandi su pipe non è implementata.

