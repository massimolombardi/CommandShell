#ifndef CCOMMANDSHELL_H
#define CCOMMANDSHELL_H

#include <map>
#include <string>
#include <cassert>
#include <stdio.h>
#include <iostream>
#include <typeinfo>
#include <typeindex>
using namespace std;

#include "cConfigurationFile.h"

/**
 * @abstract
 * Questa classe implementa una shell che fornisce una lista di comandi personalizzati, implementati
 * con funzioni scritte secondo la propria necessità.
 *
 * La classe utilizza due strutture dati interne, una stl map<string, string> per memorizzare i parametri
 * di configurazione ed una stl map<string, CMD_STATUS(*)(char* const*)> per implementare
 * la lista di metodi da invocare per i comandi.
 *
 * Tutte le funzioni di callback devono avere il tipo di ritorno CMD_STATUS, definito all'interno di questa classe,
 * che nello specifico associa il valore CMD_OK ad un comando completato con successo ed un valore CMD_ERROR ad un
 * comando completato con esito negativo.
 *
 * E' possibile definire comandi che vengono eseguiti in background, impostando a TRUE il flag apposito del metodo
 * addCommand. Inoltre, se impostato sul file, è possibile eseguire tutti i comandi presenti nell'ambiente Linux sul
 * quale viene eseguita la shell.
 *
 * Il file di configurazione deve essere un file di testo avente la seguente struttura:
 *  - deve contenere delle coppie <nome, valore>
 *  - ognuna di queste coppie deve essere scritta nel file di testo nel modo seguente:
 *      - <bol>[<delimiter>]<nome><delimiter>[<delimiter>]<valore><eol>, dove:
 *      - <bol> rappresenta l'inizio riga (beginning of line)
 *      - <nome> deve essere una stringa alfanumerica che non inizia per '#' e non contiene spazi
 *      - <valore> deve essere una stringa alfanumerica non contenente spazi
 *      - <delimiter> puo' essere uno dei seguenti caratteri: ' ' oppure '\t'
 *      - <eol> rappresenta la fine della riga (end of line)
 *  - Ogni riga che inizia con il carattere '#' viene trascurata (in tal modo si possono inserire commenti)
 *  - Ogni riga vuota viene ignorata
 *
 * Il file di help viene letto all'avvio. E' possibile compilarlo tenendo conto delle seguenti regole:
 *              il testo racchiuso tra $testo$ viene evidenziato con SHELL_COMMAND_COLOR mentre il testo racchiuso
 *              tra £testo£ viene evidenziato con SHELL_COMMAND_ARG_COLOR.
 *              Il file viene letto e stampato interamente, saltando esclusivamente le linee che iniziano con #.
 *
 * @endabstract
 *
 * @author Massimo Lombardi
 * @version 0.2
 * @since 09/07/2019
 *
 * @bug L'output su standard output dei comandi personalizzati che vengono eseguiti in background disturba l'esecuzione della shell e viene interpretato come un comando
 * @bug L'input dei comandi da pipe non funziona correttamente.
 * @bug La redirezione dell'output dei comandi su pipe non è implementata.
 */

#define ITALIAN_SHELL

#define LINUX_SHELL_INTEGRATION

#define MAX_OUTCMD_DIM 4096
#define MAX_CMD_DIM 128
#define MAX_HELP_DIM 4096

#define SHELL_NAME "CommandsShell"

#define HELP_DIR "config/help/"
#define CONFIG_FILE_NAME "config/shellConfig.cfg"
#define CMD_DETAILED_HELP_ARG "--help"
#define HELP_NOT_FOUND_MESSAGE "File di help non trovato"

#define DEFAULT_IN_PIPE "inPipe"
#define DEFAULT_OUT_PIPE "outPipe"
#define DEFAULT_PIPED_SHELL false
#define DEFAULT_PIPED_SHELL_OUTPUT_REDIRECT false
#define DEFAULT_TERMINAL_COLOR_ENABLED true
#define DEFAULT_LINUX_SHELL_INTEGRATION true

#define SHELL_COMMAND_COLOR 3
#define SHELL_COMMAND_ARG_COLOR 4
#define SHELL_COLOR_END "\033[0m"


//Defines per i messaggi e gli errori utilizzati in output

#ifdef ENGLISH_SHELL

#define MSG_CONFIG_LOADED "*** Configuration File Loaded ***"
#define MSG_WELCOME " waiting for command"
#define MSG_DUMP_CFG "Dump Configuration Values"
#define MSG_SHELL_CLOSED "Shell closed correctly"

#define ERROR_PIPE_CREATION "Error creating message queue "
#define ERROR_CONFIG_LOADED "Error loading configuration file"
#define ERROR_SHELL_INIT "Shell need to be initialized [.shellInit()]"
#define ERROR_CMD_NOT_FOUND "Command not found"
#define ERROR_CALLBACK_TYPE "Callback function not valid. return type shall be CMD_STATUS"
#define ERROR_ARG_NOT_VALID "Argument not valid"
#define ERROR_FORK_CHILD "Failed forking child"
#define ERROR_PIPE_INIT "Pipe could not be initialized"
#define ERROR_CMD_EXEC_1 "Could not execute command 1"
#define ERROR_CMD_EXEC_2 "Could not execute command 2"
#define ERROR_EXEC_BG_CMD "Error executing command"
#define ERROR_IN_PIPE "Error opening In-Pipe"
#define ERROR_OUT_PIPE "Error opening Out-Pipe"
#define ERROR_CFG_COLOR_DELIMITATOR "Warning: color markup in help file not closed!"
#define ERROR_DOS_FORMAT_FILE "WARNING: Configuration file in DOS format: "

#endif

#ifdef ITALIAN_SHELL

#define MSG_CONFIG_LOADED "*** File di configurazione caricato ***"
#define MSG_WELCOME " in attesa di un comando"
#define MSG_DUMP_CFG "Riepilogo dei parametri di configurazione"
#define MSG_SHELL_CLOSED "Shell terminata con successo"

#define ERROR_PIPE_CREATION "Errore di creazione della coda "
#define ERROR_CONFIG_LOADED "Errore nel caricamento del file di configurazione"
#define ERROR_SHELL_INIT "La shell deve essere inizializzata [.shellInit()]"
#define ERROR_CMD_NOT_FOUND "Il Comando non Esiste"
#define ERROR_CALLBACK_TYPE "Funzione di Callback non ammessa. Il ritorno deve essere di tipo CMD_STATUS"
#define ERROR_ARG_NOT_VALID "Argomento non valido"
#define ERROR_FORK_CHILD "Errore creazione processo figlio"
#define ERROR_PIPE_INIT "La pipe non puo' essere inizializzata"
#define ERROR_CMD_EXEC_1 "Impossibile eseguire il comando 1"
#define ERROR_CMD_EXEC_2 "Impossibile eseguire in comando 2"
#define ERROR_EXEC_BG_CMD "Errore nell'esecuzione del comando"
#define ERROR_IN_PIPE "Errore nell'apertura della pipe di ingresso "
#define ERROR_OUT_PIPE "Errore nell'apertura della pipe di uscita"
#define ERROR_CFG_COLOR_DELIMITATOR "Attenzione: non è stato chiuso il tag del marcatore di colore nel file di Help!"
#define ERROR_DOS_FORMAT_FILE "Attenzione File di configurazione no formato DOS: "

#endif

enum CMD_STATUS {
    CMD_NOT_FOUND = -1,
    CMD_OK = 0,
    CMD_ERROR = 1,
    ARG_NOT_VALID = 2
};

//enum OUTPUT_MSG_TYPE {
//    OK_MSG = 0,
//    ERROR_MSG = -1
//};

class CCommandShell {

    public:
        /**
         *  @brief Alloca un oggetto Shell.
         */
        CCommandShell();


        /**
         *  @brief Alloca un oggetto Shell specificando il file di configurazione.
         */
        CCommandShell(string configFileName);


        /**
         *  @brief Distruttore.
         */
        ~CCommandShell();


        /**
         *  @brief Inizializza un oggetto Shell.
         */
        void shellInit();


        /**
         *  @brief Avvia la Shell e vi cede il controllo.
         *
         *  @return La funzione ritorna void.
         *
         *  Questa funzione avvia la Shell creata e vi lascia il controllo
         *  dell'applicazione. Il metodo è bloccante.
         */
        void runShell();


        /**
         *  @brief Inserisce nella mappa un nuovo comando.
         *
         *  @param  string s
         *  @param CMD_STATUS (*fun)(char* const*)
         *  @param bool bRunInThread
         *
         *  @return La funzione ritorna void.
         *
         *  Questa funzione inserisce un nuovo comando all'interno della mappa.
         *  NOTA: possono essere inserite soltanto Callback con ritorno di tipo
         *  CMD_STATUS ovvero che seguono questo pattern:
         *              CMD_STATUS myFunction(char* const args[]);
         *  in quanto ritorni di tipo diverso da CMD_STATUS non sono gestiti.
         *  Inoltre si deve utilizzare come valore di ritorno CMD_OK in caso di successo
         *  e CMD_ERROR in caso di errore. Il valore CMD_NOT_FOUND viene utilizzato dalla
         *  classe  stessa per condizioni di errore. Altri valori di ritorno non sono ammessi
         *  ne sono gestiti.
         */
        void addCommand(string s, CMD_STATUS (*fun)(char* const*), bool bRunInThread);


        /**
         *  @brief Cerca un comando utilizzando la chiave ed esegue il relativo callback.
         *
         *  @param  string s
         *  @param char* const* args
         *
         *  @return Ritorna il valore del metodo di Callback utilizzato oppure CMD_NOT_FOUND
         *  se il comando non è stato trovato.
         *
         *  Questa funzione cerca un comando all'interno della mappa ed esegue il
         *  relativo metodo associato in caso di successo. In caso di comando non trovato
         *  restituisce false. Non viene effettuato nessun controllo sulla validità dei parametri
         *  che deve essere effettuato dal metodo di callback stesso.
         */
        CMD_STATUS callCommand(string s,  char* const* args);

    private:
        int m_iInPipeFd;
        int m_iOutPipeFd;
        bool m_bPipedShell;
        bool m_bPipedOutput;
        string m_sInPipeName;
        string m_sOutPipeName;

        bool m_bShellRunning;
        bool m_bShellInitOk;
        bool m_bLinuxShellIntegration;

        string m_sShellName;
        string m_configurationFileName;
        string m_helpDirName;        
        char m_sShellInputMarkup[MAX_CMD_DIM] = {0};

        bool m_bColorTerminalEnabled;
        static string availableColors[16];
        unsigned int m_iCmdColor;
        unsigned int m_iArgsColor;

        char m_sInputString[MAX_CMD_DIM];
        char* m_cpParsedArgs[MAX_CMD_DIM];
        char* m_cpParsedArgsPiped[MAX_CMD_DIM];

        map<string, pair<CMD_STATUS (*)(char* const*), bool>> m_CommandList;
        map<string, string> m_ConfigValues;

        //Command Execution Methods
        CMD_STATUS CommandHandler();
        bool builtInCommandHandler();
        void execLinuxCmd();
        void execLinuxPipedCmd();
        void execCmdInBackground(CMD_STATUS (*fun)(char* const*));

        //Built-In Commands
        CMD_STATUS helpCommand();
        CMD_STATUS closeShellCommand();
        CMD_STATUS ChDirCommand();

        //Command Parsing Methods
        void getCommand();
        int inputStringProcess();
        int parsePipe(char* str, char** strpiped);
        void parseSpace(char* str, char** parsed);
        void clearBuffer();

        //Command's output pipe
        bool readPipe();
        bool writePipe(string msg);
        void printMessage(string msg, OUTPUT_MSG_TYPE type);

        //Built-In Command Support Methods
        bool loadCmdHelpMsg(string fileName);
        void colorParse(char message[]);

        //Load Config File Methods
        bool loadConfiguration();

};

#endif
