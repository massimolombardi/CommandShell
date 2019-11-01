#include "fcntl.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include <fstream>
#include <strstream>
#include<sys/wait.h>
#include<sys/types.h>
#include<readline/readline.h>
#include<readline/history.h>
using namespace std;

#include "CCommandShell.h"

string CCommandShell::availableColors[16] = {"[30m", "[31m", "[32m", "[33m", "[34m", "[35m", "[36m", "[37m",
                                                "[1m\033[30m", "[1m\033[31m", "[1m\033[32m", "[1m\033[33m",
                                                "[1m\033[34m", "[1m\033[35m", "[1m\033[36m", "[1m\033[37m"};


CCommandShell::CCommandShell() {

    m_sInPipeName = DEFAULT_IN_PIPE;
    m_sOutPipeName = DEFAULT_OUT_PIPE;

    m_bPipedShell = false;
    m_bPipedOutput = false;
    m_bShellInitOk = false;
    m_bShellRunning = false;
    m_bColorTerminalEnabled = false;
    m_bLinuxShellIntegration = false;

    m_sInputString[0] = '\0';

    m_iCmdColor = SHELL_COMMAND_COLOR;
    m_iArgsColor = SHELL_COMMAND_ARG_COLOR;

    m_sShellName = SHELL_NAME;
    m_helpDirName = HELP_DIR;
    m_configurationFileName = CONFIG_FILE_NAME;
}


CCommandShell::CCommandShell(string configFileName) {

    m_sInPipeName = DEFAULT_IN_PIPE;
    m_sOutPipeName = DEFAULT_OUT_PIPE;

    m_bPipedShell = false;
    m_bPipedOutput = false;
    m_bShellInitOk = false;
    m_bShellRunning = false;
    m_bColorTerminalEnabled = false;
    m_bLinuxShellIntegration = false;

    m_sInputString[0] = '\0';

    m_iCmdColor = SHELL_COMMAND_COLOR;
    m_iArgsColor = SHELL_COMMAND_ARG_COLOR;

    m_sShellName = SHELL_NAME;
    m_helpDirName = HELP_DIR;
    m_configurationFileName = configFileName;
}


CCommandShell::~CCommandShell() {
    close(m_iInPipeFd);
    close(m_iOutPipeFd);
}


void CCommandShell::shellInit() {

    if(loadConfiguration()) {

        printMessage(MSG_CONFIG_LOADED, OK_MSG);

        char* username = getenv("USER");

        if(m_bColorTerminalEnabled) {
            strcat(m_sShellInputMarkup, "\033[1;33m");
            strcat(m_sShellInputMarkup, username);
            strcat(m_sShellInputMarkup, "@");
            strcat(m_sShellInputMarkup, m_sShellName.c_str());
            strcat(m_sShellInputMarkup, "\033[0m");
            strcat(m_sShellInputMarkup, "\033[1;34m $ \033[0m");
        }
        else {
            strcat(m_sShellInputMarkup, username);
            strcat(m_sShellInputMarkup, "@");
            strcat(m_sShellInputMarkup, m_sShellName.c_str());
            strcat(m_sShellInputMarkup, " $ ");
        }

        if(m_bPipedShell) {

            m_iInPipeFd = open(m_sInPipeName.c_str(), O_CREAT | O_RDONLY);

            if(m_iInPipeFd < 0) {
                string msg = ERROR_PIPE_CREATION + m_sInPipeName + ": " + strerror(errno);
                printMessage(msg, ERROR_MSG);
                m_bShellInitOk = false;
            }

            m_iOutPipeFd = open(m_sOutPipeName.c_str(), O_CREAT | O_WRONLY);

            if(m_iOutPipeFd < 0) {
                string msg = ERROR_PIPE_CREATION + m_sOutPipeName + ": " + strerror(errno);
                printMessage(msg, ERROR_MSG);
                m_bShellInitOk = false;
            }
        }

        m_bShellInitOk = true;
    }
    else
        printMessage(ERROR_CONFIG_LOADED, ERROR_MSG);
}


void CCommandShell::runShell() {

    printMessage(MSG_WELCOME, OK_MSG);

    if(m_bShellInitOk)
        m_bShellRunning = true;
    else {
        printMessage(ERROR_SHELL_INIT, ERROR_MSG);
        return;
    }

    while(m_bShellRunning) {

            getCommand();
            int execMode = inputStringProcess();

            if(m_bLinuxShellIntegration) {
                //Eseguo un comando del SO
                if(execMode == 1)
                    execLinuxCmd();

                //Eseguo un comando del SO con Pipe
                else if(execMode == 2)
                    execLinuxPipedCmd();
            }
            else {
                if(execMode != 0)
                    printMessage(ERROR_CMD_NOT_FOUND, ERROR_MSG);
            }
    }
    printMessage(MSG_SHELL_CLOSED, OK_MSG);
}


void CCommandShell::addCommand(string s, CMD_STATUS (*fun)(char* const*), bool bRunInThread) {

    string functionType = typeid(fun).name();

    if(functionType.rfind("PF10CMD_STATUS", 0) != 0)
        printMessage(ERROR_CALLBACK_TYPE, ERROR_MSG);
    else {
        pair<CMD_STATUS (*)(char* const*), bool> tmpPair = make_pair(fun, bRunInThread);
        m_CommandList.insert(make_pair(s, tmpPair));
    }
}


/******************************************* Private Methods **********************************************/

CMD_STATUS CCommandShell::CommandHandler() {
    if(builtInCommandHandler())
        return CMD_OK;
    else
        return callCommand(m_cpParsedArgs[0], m_cpParsedArgs);
}


bool CCommandShell::builtInCommandHandler() {

    if(strcmp(m_cpParsedArgs[0], "help") == 0) {
        this->helpCommand();
        return true;
    }

    if(strcmp(m_cpParsedArgs[0], "closeShell") == 0) {
        this->closeShellCommand();
        return true;
    }

    if(strcmp(m_cpParsedArgs[0], "cd") == 0) {
        this->ChDirCommand();
        return true;
    }

    return false;
}


CMD_STATUS CCommandShell::callCommand(string s,  char* const* args) {

    auto mapIter = m_CommandList.find(s);

    if(mapIter != m_CommandList.end()) {

        if(m_cpParsedArgs[1] != NULL)
            if(strcmp(m_cpParsedArgs[1], CMD_DETAILED_HELP_ARG) == 0) {
                loadCmdHelpMsg(s);
                return CMD_OK;
            }

        CMD_STATUS(*typeCastedFun)(char* const*) = mapIter->second.first;

        if(mapIter->second.second == true) {
            execCmdInBackground(typeCastedFun);
            return CMD_OK;
        }
        else
            return typeCastedFun(args);
    }
    else {
        return CMD_NOT_FOUND;
    }
}


/****************************** Built-In Commands ******************************/

CMD_STATUS CCommandShell::helpCommand() {
    loadCmdHelpMsg("help");
    return CMD_OK;
}


CMD_STATUS CCommandShell::closeShellCommand() {
    if(m_cpParsedArgs[1] != NULL && strcmp(m_cpParsedArgs[1], CMD_DETAILED_HELP_ARG) != 0) {
        printMessage(ERROR_ARG_NOT_VALID, ERROR_MSG);
        return ARG_NOT_VALID;
    }
    if(strcmp(m_cpParsedArgs[1], CMD_DETAILED_HELP_ARG) == 0)
        loadCmdHelpMsg("closeShell");
    else
        m_bShellRunning = false;
    return CMD_OK;
}


CMD_STATUS CCommandShell::ChDirCommand() {
    chdir(m_cpParsedArgs[1]);
    return CMD_OK;
}


/****************************** Parse Commands Methods ******************************/

void CCommandShell::getCommand() {

    char *inBuffer;

    clearBuffer();

    if(!m_bPipedShell) {
        inBuffer = readline(m_sShellInputMarkup);

        if (strlen(inBuffer) != 0) {
            add_history(inBuffer);
            strcpy(m_sInputString, inBuffer);
            return;
        }
        else
            return;
    }
    else
        readPipe();
}


int CCommandShell::inputStringProcess() {

    int piped = 0;
    char* strpiped[2];

    if(m_sInputString[0] == '\0' || m_sInputString == NULL)
        return 0;

    piped = parsePipe(m_sInputString, strpiped);

    if(piped) {
        parseSpace(strpiped[0], m_cpParsedArgs);
        parseSpace(strpiped[1], m_cpParsedArgsPiped);
    }
    else {
        parseSpace(m_sInputString, m_cpParsedArgs);
    }

    if(CommandHandler() != CMD_NOT_FOUND)
        return 0;
    else
        return 1 + piped;
}


int CCommandShell::parsePipe(char* str, char** strpiped) {

    for(int i=0; i<2; i++) {
        strpiped[i] = strsep(&str, "|");
        if(strpiped[i] == NULL)
            break;
    }

    if (strpiped[1] == NULL)
        return 0;
    else
        return 1;
}


void CCommandShell::parseSpace(char* str, char** parsed) {

    for (int i=0; i<MAX_CMD_DIM; i++) {

        parsed[i] = strsep(&str, " ");

        if(parsed[i] == NULL)
            break;
        if (strlen(parsed[i]) == 0)
            i--;
    }
}


void CCommandShell::clearBuffer() {
    m_sInputString[0] = '\0';
    m_cpParsedArgs[0] = '\0';
    m_cpParsedArgsPiped[0] = '\0';
}


void CCommandShell::execLinuxCmd() {

    pid_t pid = fork();

    if(pid == -1) {
        printMessage(ERROR_FORK_CHILD, ERROR_MSG);
        return;
    }
    else if(pid == 0) {
        if (execvp(m_cpParsedArgs[0], m_cpParsedArgs) < 0) {
            printMessage(ERROR_CMD_NOT_FOUND, ERROR_MSG);
        }
        exit(0);
    }
    else {
        wait(NULL);
        return;
    }
}


void CCommandShell::execLinuxPipedCmd() {

    // 0 is read end, 1 is write end
    int pipefd[2];
    pid_t p1, p2;

    if(pipe(pipefd) < 0) {
        printMessage(ERROR_PIPE_INIT, ERROR_MSG);
        return;
    }

    p1 = fork();

    if(p1 < 0) {
        printMessage(ERROR_FORK_CHILD, ERROR_MSG);
        return;
    }

    if(p1 == 0) {

        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        if (execvp(m_cpParsedArgs[0], m_cpParsedArgs) < 0) {
            printMessage(ERROR_CMD_EXEC_1, ERROR_MSG);
            exit(0);
        }
    }
    else {
        p2 = fork();
        if (p2 < 0) {
            printMessage(ERROR_FORK_CHILD, ERROR_MSG);
            return;
        }
        if (p2 == 0) {
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
            if (execvp(m_cpParsedArgsPiped[0], m_cpParsedArgsPiped) < 0) {
                printMessage(ERROR_CMD_EXEC_2, ERROR_MSG);
                exit(0);
            }
        }
        else {
            wait(NULL);
            wait(NULL);
        }
    }
}


void CCommandShell::execCmdInBackground(CMD_STATUS (*fun)(char* const*)) {

    int pipefd[2];

    pid_t pid = fork();

    if(pid == -1) {
        printMessage(ERROR_FORK_CHILD, ERROR_MSG);
        return;
    }
    else if(pid == 0) {

        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        if (fun(m_cpParsedArgs) != CMD_OK) {
            printMessage(ERROR_EXEC_BG_CMD, ERROR_MSG);
        }
        exit(0);
    }
    else {
        return;
    }
}


/*************************** Command's output pipe Methods ***************************/

bool CCommandShell::readPipe() {

    int i = 0;
    char c = 0;
    char cBuffer[MAX_CMD_DIM];

    while(c != '\n') {
        read(m_iInPipeFd, &c, 1);
        if(c != 0 && c != EOF) {
            cBuffer[i] = c;
            i++;
        }
    }

    i = 0;

    if(cBuffer != NULL) {
        strcpy(m_sInputString, cBuffer);
        printf("%s\n", cBuffer);
    }

    return true;
}


bool CCommandShell::writePipe(string msg) {

    write(m_iInPipeFd, msg.c_str(), MAX_OUTCMD_DIM);

    return true;
}


void CCommandShell::printMessage(string msg, OUTPUT_MSG_TYPE type) {

    if(m_bPipedOutput) {
        writePipe(msg);
    }
    else {
        if(type == OK_MSG) {
            cout << msg << endl;
            return;
        }

        if(type == ERROR_MSG) {
            cout << msg << endl;
            return;
        }
    }

    return;
}

/****************************** Built-In Command Support Private Methods ******************************/

bool CCommandShell::loadCmdHelpMsg(string fileName) {

    bool bResult = true;
    char tmpMsg[MAX_HELP_DIM] = "";
    bool bWarning_FileInDosFormat = false;

    fileName = m_helpDirName + fileName;

    if(fileName == "")
        bResult = false;

    ifstream fin(fileName.c_str());

    if(fin.bad() || fin.fail())
        bResult = false;

    char line[256];

    while(bResult && !fin.eof()) {
        fin.getline(line, 256);

        char *pPos = strchr(line,0x0D);
        if(pPos != NULL) {
            *pPos=0x00;
            bWarning_FileInDosFormat = true;
        }

        if(line[0] == '#' || strlen(line) == 0)
            continue;

        strcat(tmpMsg, line);
        strcat(tmpMsg, "\n");
    }
    colorParse(tmpMsg);

    if(bWarning_FileInDosFormat == true) {
        string msg = ERROR_DOS_FORMAT_FILE + fileName;
        printMessage(msg, ERROR_MSG);
    }

    fin.close();

    if(bResult)
        printMessage(tmpMsg, OK_MSG);
    else
        printMessage(HELP_NOT_FOUND_MESSAGE, ERROR_MSG);

    return bResult;
}


void CCommandShell::colorParse(char message[]) {

    bool closeCmd = false;
    bool closeArgs = false;

    if(m_bColorTerminalEnabled) {
        string tmp = message;

        for(unsigned int i=0; i<tmp.length(); i++) {
            if(tmp.at(i) == '$') {
                if(closeCmd) {
                    closeCmd = false;
                    tmp.replace(i, 1, SHELL_COLOR_END);
                }
                else {
                    closeCmd = true;
                    tmp.replace(i, 1, '\33'+availableColors[m_iCmdColor]);
                }
            }
            if(tmp.at(i) == '%') {
                if(closeArgs) {
                    closeArgs = false;
                    tmp.replace(i, 1, SHELL_COLOR_END);
                }
                else {
                    closeArgs = true;
                    tmp.replace(i, 1, '\33'+availableColors[m_iArgsColor]);
                }
            }
        }
        strcpy(message, tmp.c_str());
    }

    if(closeArgs || closeCmd)
        printMessage(ERROR_CFG_COLOR_DELIMITATOR, ERROR_MSG);

    return;
}


/****************************** Config Param Private Methods ******************************/

bool CCommandShell::loadConfiguration() {

    cConfigurationFile config = cConfigurationFile(m_configurationFileName.c_str());

    if(!config.loadConfigFile())
        return false;

    m_helpDirName = config.getStringValue("SHELL_CMD_HELP_DIR");
    if(m_helpDirName == "")
        m_helpDirName = HELP_DIR;

    if(!config.getUnsignedIntValue("SHELL_COMMAND_COLOR", &m_iCmdColor))
        m_iCmdColor = SHELL_COMMAND_COLOR;

    if(!config.getUnsignedIntValue("SHELL_COMMAND_ARG_COLOR", &m_iArgsColor))
        m_iArgsColor = SHELL_COMMAND_ARG_COLOR;

    m_sShellName = config.getStringValue("SHELL_NAME");
    if(m_sShellName == "")
        m_sShellName = SHELL_NAME;

    m_sInPipeName = config.getStringValue("IN_PIPE_NAME");
    if(m_sInPipeName == "")
        m_sInPipeName = DEFAULT_IN_PIPE;

    m_sOutPipeName = config.getStringValue("OUT_PIPE_NAME");
    if(m_sOutPipeName == "")
        m_sOutPipeName = DEFAULT_OUT_PIPE;

    if(!config.getBoolValue("PIPED_SHELL", &m_bPipedShell))
        m_bPipedShell = DEFAULT_PIPED_SHELL;

    if(!config.getBoolValue("PIPED_SHELL_OUTPUT_REDIRECT", &m_bPipedOutput))
        m_bPipedOutput = DEFAULT_PIPED_SHELL_OUTPUT_REDIRECT;

    if(!config.getBoolValue("SHELL_TERMINAL_COLOR_ENABLED", &m_bColorTerminalEnabled))
        m_bColorTerminalEnabled = DEFAULT_TERMINAL_COLOR_ENABLED;

    if(!config.getBoolValue("SHELL_LINUX_INTEGRATION", &m_bLinuxShellIntegration))
        m_bLinuxShellIntegration = DEFAULT_LINUX_SHELL_INTEGRATION;

    return true;
}
