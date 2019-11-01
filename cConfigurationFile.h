#ifndef CCONFIGURATIONFILE_H
#define CCONFIGURATIONFILE_H

#include <map>
#include <string>

/**
 * @abstract
 *
 * Questa classe permette di leggere delle coppie chiave-valore da un file di configurazione personalizzabile.
 * Sono disponibili inoltre alcuni metodi per la lettura di valori del tipo specificato.
 * Utilizzando i metodi per leggere il dato come tipo fondamentale, vengono effettuati automaticamente tutti i
 * controlli di rappresentabilità ed il rispettivo metodo restituisce gli eventuali casi di errore.
 *
 * I tipi disponibili sono:
 *  string
 *  int
 *  unsigned int
 *  float
 *  bool
 *
 * Per il tipo float è possibile utilizzare entrambe le notazioni decimale e ad esponente.
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
 * @endabstract
 *
 * @author Massimo Lombardi
 * @version 0.1
 * @since 28/10/2019
 *
 * @bug
 *
 */

#define DEFAULT_FILE_NAME "configuration.cfg"
#define DEFAULT_MAX_CONFIG_VALUE 256

#define ITALIAN_MSG

#ifdef ENGLISH_MSG
#define MSG_CONFIG_LOADED "*** Configuration File Loaded ***"
#define MSG_DUMP_CFG "Dump Configuration Values"

#define ERROR_CONFIG_LOADED "Error loading configuration file"
#define ERROR_DOS_FORMAT_FILE "WARNING: Configuration file in DOS format: "

#define ERROR_NOT_DECIMAL_NUMBER "Read value is not a decimal number"
#define ERROR_OTHER_CHAR_AFTER_END "Extra characters at the end of input"
#define ERROR_TOO_LONG_NUMBER "Read value is out of range for long type"
#define ERROR_TOO_LONG_FOR_INT "Read value is out of range for int type"
#define ERROR_TOO_LONG_FOR_UINT "Read value is out of range for unsigned int type"
#define ERROR_TOO_LONG_FOR_FLOAT "Read value is out of range for float type"
#define ERROR_NOT_VALID_VALUE "Read value is out of range for specified type"
#define ERROR_FILE_NOT_FOUND "File not found"
#define ERROR_READING_FILE "Error reading configuration file"
#endif

#ifdef ITALIAN_MSG
#define MSG_CONFIG_LOADED "*** File di configurazione caricato ***"
#define MSG_DUMP_CFG "Riepilogo dei parametri di configurazione"

#define ERROR_CONFIG_LOADED "Errore nel caricamento del file di configurazione"
#define ERROR_DOS_FORMAT_FILE "Attenzione File di configurazione no formato DOS: "

#define ERROR_NOT_DECIMAL_NUMBER "Il valore letto non è un numero decimale"
#define ERROR_OTHER_CHAR_AFTER_END "Trovati altri caratteri all'interno del valore letto"
#define ERROR_TOO_LONG_NUMBER "Il valore letto e' out of range per il tipo long"
#define ERROR_TOO_LONG_FOR_INT "Il valore letto non è rappresentabile su un intero"
#define ERROR_TOO_LONG_FOR_UINT "Il valore letto non è rappresentabile su un intero senza segno"
#define ERROR_TOO_LONG_FOR_FLOAT "Il valore letto non è rappresentabile su un float"
#define ERROR_NOT_VALID_VALUE "Il valore letto e' out of range per il tipo specificato"
#define ERROR_FILE_NOT_FOUND "Il file specificato non esiste"
#define ERROR_READING_FILE "Errore durante la lettura del file"
#endif

#define DEFAULT_BASE 10
#define DEFAULT_STRING_VALUE ""
#define DEFAULT_BOOL_VALUE false
#define DEFAULT_INT_VALUE 0
#define DEFAULT_UNSIGNEDINT_VALUE 0
#define DEFAULT_FLOAT_VALUE 0.0


enum OUTPUT_MSG_TYPE {
    OK_MSG = 0,
    ERROR_MSG = -1
};


class cConfigurationFile {

    public:

        /**
         *  @brief Alloca un oggetto ConfigurationFile permettendo di specificare, eventualmente, lo stato
         *          dei messaggi di errore.
         */
        cConfigurationFile(bool bDisableMsgOutput = false);


        /**
         *  @brief Alloca un oggetto ConfigurationFile  permettendo di specificare il file di configurazione
         *          ed, eventualmente, lo stato dei messaggi di errore.
         */
        cConfigurationFile(const char* sFileName, bool bDisableMsgOutput = false);


        /**
         *  @brief Carica i dati dal file di configurazione.
         *
         *  @return La funzione ritorna true se il file è stato caricato correttamente.
         *
         *  Questa funzione riempie la mappa di coppie chiave-valore leggendo il file di
         *  configurazione indicato. E' necessario invocare questa funzione per poter utilizzare
         *  le funzioni per il recupero dei valori indicati nel file, altrimenti la mappa risulta
         *  vuota.
         *
         */
        bool loadConfigFile();


        /**
         *  @brief Elimina i dati di configurazione letti dal file.
         *
         *  @return La funzione ritorna void.
         *
         *  Questa funzione elimina dalla mappa tutti i dati di configurazione letti da file
         *  attraverso la loadConfigFile().
         *
         */
        void clearConfigValues();


        /**
         *  @brief Stampa su stdout i dati contenuti nella mappa.
         *
         *  @return La funzione ritorna void.
         *
         *  Questa funzione stampa su stdout tutto il contenuto della mappa di coppie
         *  chiave valore caricate dal file di configurazione.
         *
         */
        void dumpConfiguration();


        /**
         *  @brief Recupera il valore associato a key dalla mappa.
         *
         *  @param  const char* key
         *
         *  @return La funzione ritorna il valore string contenuto nella mappa oppure DEFAULT_STRING_VALUE
         *          in caso key non sia presente all'interno della mappa.
         *
         *  Questa funzione permette di recuperare il valore associato alla chiave key specificata.
         *
         */
        string getStringValue(const char* key);


        /**
         *  @brief Recupera il valore associato a key dalla mappa.
         *
         *  @param  const char* key
         *  @param  string* value
         *
         *  @return La funzione ritorna true se key è presente all'interno della mappa, false altrimenti.
         *
         *  Questa funzione permette di recuperare il valore associato alla chiave key specificata.
         *  Il valore, se trovato viene copiato all'interno del parametro value.
         *
         */
        bool getStringValue(const char* key, string* value) ;


        /**
         *  @brief Recupera il valore associato a key dalla mappa.
         *
         *  @param  const char* key
         *
         *  @return La funzione ritorna il valore bool contenuto nella mappa oppure DEFAULT_BOOL_VALUE
         *          in caso key non sia presente all'interno della mappa.
         *
         *  Questa funzione permette di recuperare il valore associato alla chiave key specificata.
         *
         */
        bool getBoolValue(const char* key);


        /**
         *  @brief Recupera il valore associato a key dalla mappa.
         *
         *  @param  const char* key
         *  @param  bool* value
         *
         *  @return La funzione ritorna true se key è presente all'interno della mappa, false altrimenti.
         *
         *  Questa funzione permette di recuperare il valore associato alla chiave key specificata.
         *  Il valore, se trovato viene copiato all'interno del parametro value.
         *
         */
        bool getBoolValue(const char* key, bool* value);


        /**
         *  @brief Recupera il valore associato a key dalla mappa.
         *
         *  @param  const char* key
         *
         *  @return La funzione ritorna il valore int contenuto nella mappa oppure DEFAULT_INT_VALUE
         *          in caso key non sia presente all'interno della mappa.
         *
         *  Questa funzione permette di recuperare il valore associato alla chiave key specificata.
         *
         */
        int getIntValue(const char* key);


        /**
         *  @brief Recupera il valore associato a key dalla mappa.
         *
         *  @param  const char* key
         *  @param  int* value
         *
         *  @return La funzione ritorna true se key è presente all'interno della mappa, false altrimenti.
         *
         *  Questa funzione permette di recuperare il valore associato alla chiave key specificata.
         *  Il valore, se trovato viene copiato all'interno del parametro value.
         *
         */
        bool getIntValue(const char* key, int* value);


        /**
         *  @brief Recupera il valore associato a key dalla mappa.
         *
         *  @param  const char* key
         *
         *  @return La funzione ritorna il valore unsigned int contenuto nella mappa oppure DEFAULT_UNSIGNEDINT_VALUE
         *          in caso key non sia presente all'interno della mappa.
         *
         *  Questa funzione permette di recuperare il valore associato alla chiave key specificata.
         *
         */
        unsigned int getUnsignedIntValue(const char* key);


        /**
         *  @brief Recupera il valore associato a key dalla mappa.
         *
         *  @param  const char* key
         *  @param  unsigned int* value
         *
         *  @return La funzione ritorna true se key è presente all'interno della mappa, false altrimenti.
         *
         *  Questa funzione permette di recuperare il valore associato alla chiave key specificata.
         *  Il valore, se trovato viene copiato all'interno del parametro value.
         *
         */
        bool getUnsignedIntValue(const char* key, unsigned int* value);


        /**
         *  @brief Recupera il valore associato a key dalla mappa.
         *
         *  @param  const char* key
         *
         *  @return La funzione ritorna il valore float contenuto nella mappa oppure DEFAULT_FLOAT_VALUE
         *          in caso key non sia presente all'interno della mappa.
         *
         *  Questa funzione permette di recuperare il valore associato alla chiave key specificata.
         *
         */
        float getFloatValue(const char* key);


        /**
         *  @brief Recupera il valore associato a key dalla mappa.
         *
         *  @param  const char* key
         *  @param  float* value
         *
         *  @return La funzione ritorna true se key è presente all'interno della mappa, false altrimenti.
         *
         *  Questa funzione permette di recuperare il valore associato alla chiave key specificata.
         *  Il valore, se trovato viene copiato all'interno del parametro value.
         *
         */
        bool getFloatValue(const char* key, float* value);


    private:
        bool m_bDisableMsgOutput;
        string m_sFileName;
        map<string, string> m_ConfigValues;

        //Output Messages Methods
        void printMessage(string msg, OUTPUT_MSG_TYPE type);
        void printMessage(const char* key, string msg, OUTPUT_MSG_TYPE type);
};


#endif
