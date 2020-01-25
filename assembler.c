#include "assembler.h"

/**Passo 2**/
void assembler_pass_two(const char* nomeArquivo)
{
    FILE *arquivoEntrada, *arquivoSaida;                // Ponteiro para acessar o arquivo criado no passo 1
    char linha[100];                                    // String que le arquivo de entrada, linha por linha
    char* token;                                        // ponteiro separador de palavras
    char label[30] , opcode[20], operando1[5], operando2[5],num_bytes[10], constante[10];
    label[0] = '\0'; opcode[0] = '\0'; operando1[0] = '\0'; operando2[0] = '\0';num_bytes[0] = '\0';constante[0] = '\0';
    int tipoOperacao, i, endereco=0;
    arquivoEntrada = fopen(nomeArquivo, "r");           // abre arquivo .a para leitura
    const char *enderecoArquivoSaida = "tables\\saida1.mif";
    arquivoSaida = fopen(enderecoArquivoSaida,"w");  // abre arquivo para leitura e escrita no final


    //  ========== Escreve cabeçalho arquivo de saida.mif ==========
    if(arquivoSaida==NULL){
        printf("Problemas na CRIACAO do arquivo de saida\n");
        exit(1);
    }else{
        fprintf(arquivoSaida,"DEPTH = 128;\n");
        fprintf(arquivoSaida,"WIDTH = 8;\n");
        fprintf(arquivoSaida,"ADDRESS_RADIX = HEX;\n");
        fprintf(arquivoSaida,"DATA_RADIX = BIN;\n");
        fprintf(arquivoSaida,"CONTENT\n");
        fprintf(arquivoSaida,"BEGIN\n\n");
    }


    //  ========== Escreve linha a linha arquivo saida.mif ==========

    if(arquivoEntrada == NULL){                         // Se não existir arquivo de entrada, imprime erro
        printf("Problemas na CRIACAO do arquivo\n");
        system("pause");
        exit(1);
    }else{

        for(i=0;!feof(arquivoEntrada);i++)                          // Percorre o arquivo .a até a ultima linha
        {
            int countRegistrador=0, countConstantes = 0;
            label[0] = '\0'; opcode[0] = '\0'; operando1[0] = '\0'; operando2[0] = '\0';num_bytes[0] = '\0';constante[0] = '\0';

            fgets(linha, 100, arquivoEntrada);          // fgets le linha por linha
            token = strtok(linha, " ");                 // Separa a linha lida com espaços " "
            while(token != NULL){                       //  A cada iteração do laço, token aponta para proxima palavra da linha

                if(token[0]== ';')                      // Ignora comentarios
                {
                    break;
                }
                else
                {

                    if( verificaoperacao(token) == 0){                  // verifica operação retorna 0 se o token é um operando
                        tipoOperacao = getTipoOp(token);
                        strcpy(opcode,token);
                        //printf("%s \n",opcode);
                    }

                    if(token[0] =='_' && token[strlen(token)-1] !=':'){ // Verifica se é CHAMADA de uma Label, logo não tem ':'
                        strcpy(label,token);
                    }

                    if( verificaRegistrador(token) == 0){               // verifica operação retorna 0 se o token é um registrador
                        if( countRegistrador==0){                       // É o primeiro registrador? Se sim, faça:
                            strcpy(operando1,token);                    // Operando1 receber valor

                            countRegistrador ++;                        // verifica quantos registradores foram lidos na intrução
                           // printf("%s \n",operando1);
                        }else{
                            strcpy(operando2,token);                    // Caso seja o segundo registrador, operando2 receber valor
                           ///printf("%s \n",operando2);
                        }
                    }
                    if(verificaConstante(token)==0){
                        //printf("%s \n",token); ///teste cte
                        if(strcmp(opcode,".data")==0){                      // é operando de uma pseudo instrução? se sim...
                            if(countConstantes ==0){                        // É a primeira constante? Então é num_bytes
                                strcpy(num_bytes,token);
                                countConstantes++;
                                ///printf("sou num bytes %s \n",num_bytes);
                                int valor = atoi(num_bytes);
                                ///printf("meu novo valor %d\n",valor);
                                //endereco += (2*valor)-2;                    // -2 pq valor vai ser incrementado depois
                            }else{
                                strcpy(constante,token);
                            }
                        }else{                                              // é só uma constante
                            strcpy(constante,token);
                        }

                    }
                printf("%s \t",token);


                    token = strtok(NULL, " ");                          // token recebe proxima palavra da linha, identificada pelos espaços
                }

            }
                            printf("\n");



            //---ESCREVENDO INSTRUÇÃO NO ARQ DE SAIDA------------------------------------//
            geraObjCode(enderecoArquivoSaida,endereco,tipoOperacao,label,opcode,operando1,operando2,num_bytes,constante);

            endereco +=2;


         ///   printf("\n");                 // Para testes, quebra linha apos immprimir cada instrução :D
        }

    }
    fim(enderecoArquivoSaida,endereco);
    fclose(arquivoEntrada);
    fclose(arquivoSaida);
}

/**Passo 1**/
void assembler_pass_one(const char* nomeArquivo)
{
    FILE *arquivo;
    char linha[100];
    int endereco = 0;
    char* token;                            //ponteiro separardor dde palavras
    arquivo = fopen(nomeArquivo, "r");      //abre arquivo .a para leitura
    if(arquivo == NULL)                     //Verifica se o arquivo não existe
    {
        printf("Problemas na CRIACAO do arquivo\n");
        system("pause");
        exit(1);
    }
    else
    {
        while(!feof(arquivo))                   //Percorre o arquivo .a até a ultima linha
        {
            fgets(linha, 100, arquivo);         //fgets le linha por linha
            //strcpy(copiaLinha, linha);
            token = strtok(linha, " ");          //Separa a linha lida com espaços " "

            while(token != NULL)                //A cada iteração do laço, token aponta para proxima palavra da linha
            {
                if(token[0]== ';' )              //Ignora comentarios tudo que for dierente de op e label
                {
                    break;
                }
                else
                {
                    //printf("%s\n",token);                      // Teste para imprimir cada palavra lida por token

                    if(token[0] == '_' && token[strlen(token)-1] ==':' ){ // verifica se é do formato de '_LABEL:'
                        ///printf("%s\t%d\n",token,endereco);                 // Teste para ver se pegou label correntamente
                        organizaLabelTable(token, endereco);    // Procura label na tabela, se não estiver, adiciona label e endereço
                    }
                    if(endereco==56){
                        ///printf("%s\n",token);
                    }

                    token = strtok(NULL, " ");    // token recebe proxima palavra da linha, identificada pelos espaços

                }
            }
            endereco += 2;               // Incrementa endereço em 2;
        }

        assembler_pass_two(nomeArquivo);    /// INICIA PASSO 2 DO MONTADOR
    }



}

/**Organiza tabela de Labbels**/
void organizaLabelTable(char* token, int endereco){
    FILE* arquivo;
   char labelTemp[30];                 // explicar essas declarações
    int endTemp, findLabel = 1;
    arquivo = fopen("tables\\LabelTab.txt", "a+");
    token[strlen(token)-1] = '\0';

    if(arquivo == NULL){
        printf("PROBLEMAS COM A CRIACAO DA TABELA DE LABELS\n");
    }else{
        while(fscanf(arquivo,"%s %d\n", labelTemp, &endTemp) != EOF){ //Percorre a tabela de labels até o final do arquivo

            if( strncmp(labelTemp,token,strlen(labelTemp)) == 0){      // :) se encontrar label, strcmp retorna 0, e
                findLabel = 0;                      // salva que label foi encontrada
            }
        }
        if(findLabel == 1){                         // :( não encontrou label,
            //token[strlen(token)-1] = '\0';           // Remove dois pontos ':' da label
            fprintf(arquivo,"%s\t\t%d\n",token,endereco);// escreve label e o endereço no arquivo
        }
    }

}

/**ENCONTRA ENDEREÇO DA LABEL NA TABELA DE ROTULOS**/
int findLabelEnd(const char* label){
    FILE* arquivo;
    arquivo = fopen("tables\\LabelTab.txt", "r");
    char labelTemp[30];                                     /// explicar essas declarações
    int endTemp;


    if(arquivo == NULL){
        printf("PROBLEMAS COM A BUSCA NA TABELA DE LABELS\n");
    }else{
        while(fscanf(arquivo,"%s %d\n", labelTemp, &endTemp) != EOF){       // Percorre a tabela de labels até o final do arquivo
            if( strncmp(labelTemp,label,strlen(labelTemp)) == 0){           // :) se encontrar label, strcmp retorna 0, e
                ///printf("\nsomos iguais\t %s ==%s \n",labelTemp,label);
                return endTemp;                                             // retorna endereço da label
            }
             ///printf("end %d e %s\n\n",endTemp,labelTemp);
        }

    }
    return 1;                                                               // Não encontrou retorna error = 1;
}

/**Busca operador na tabela de operadore**/
int verificaoperacao(const char* token){
    FILE* arquivo;
    char operadorTemp[20], opcodeTemp[20];
    int tipoTemp;
    arquivo = fopen("tables\\opTab.txt", "r");

    if(arquivo==NULL){
        printf("PROBLEMAS COM A CRIACAO DA TABELA DE OPERACOES\n");
    }else{
        while(fscanf(arquivo,"%s %s %d\n", operadorTemp, opcodeTemp, &tipoTemp) != EOF){ //Percorre a tabela de operadores até o final do arquivo
            if( strncmp(operadorTemp,token,strlen(operadorTemp)) == 0){       // :) se encontrar operação, strcmp retorna 0, e
                return 0;                               // retorna que operação foi encontrada
            }
        }
    }
    return 1;                                           // :( Operação não foi encontrada

}

/**Busca opcode da operação na tabela de operadores**/
void findOpEnd(const char* token, char* codigo){
    FILE* arquivo;
    char operadorTemp[20], opcodeTemp[20];
    int tipoTemp;
    arquivo = fopen("tables\\opTab.txt", "r");

    if(arquivo==NULL){
        printf("PROBLEMAS COM A BUSCA DE OPERACAO NA TABELA DE OPERACOES\n");
    }else{
        while(fscanf(arquivo,"%s %s %d\n", operadorTemp, opcodeTemp, &tipoTemp) != EOF){ //Percorre a tabela de operadores até o final do arquivo
            if( (strncmp(operadorTemp,token,strlen(operadorTemp))==0) && (strncmp(operadorTemp,token,strlen(token))==0) ){ ///CarlosCorrect

                strcpy(codigo,opcodeTemp);
                ///printf("token = %s\t temp=%s\n",token,operadorTemp);
            }

        }
    }

}

/**Busca Registrador na tabela de Registradores**/
int verificaRegistrador(const char* token){
    FILE* arquivo;
    char registerTemp[10], endTemp[5];
    arquivo = fopen("tables\\registerTab.txt", "r");

    if(arquivo==NULL){
        printf("PROBLEMAS COM A CRIACAO DA TABELA DE REGISTRADORES\n");
    }else{
        while(fscanf(arquivo,"%s %s\n", registerTemp, endTemp) != EOF){ //Percorre a tabela de operadores até o final do arquivo
            if( strncmp(registerTemp,token,strlen(registerTemp)) == 0){       // :) se encontrar operação, strcmp retorna 0, e
                return 0;                               // retorna que operação foi encontrada
            }
        }
    }
    return 1;                                           // :( Operação não foi encontrada
}


/**Busca opcode do registrador na tabela de Registradores**/
void findEndegistrador(const char* token, char* codigo){
    FILE* arquivo;
    char registerTemp[10], endTemp[5];
    arquivo = fopen("tables\\registerTab.txt", "r");

    if(arquivo==NULL){
        printf("PROBLEMAS COM A CRIACAO DA TABELA DE REGISTRADORES\n");
    }else{
        while(fscanf(arquivo,"%s %s\n", registerTemp, endTemp) != EOF){ //Percorre a tabela de operadores até o final do arquivo
            if( strncmp(registerTemp,token,strlen(registerTemp)) == 0){       // :) se encontrar operação, strcmp retorna 0, e
                strcpy(codigo,endTemp);
            }
        }
    }

}

/**Retorna tipo/FORMATO da operação**/
int getTipoOp(const char* token){
    FILE* arquivo;
    char operadorTemp[20];
    char opcodeTemp[20];
    int tipoTemp;
    arquivo = fopen("tables\\opTab.txt", "r");

    if(arquivo==NULL){
        printf("PROBLEMAS COM A BUSCA OPERACOES\n");
    }else{
        while(fscanf(arquivo,"%s\t%s\t%d\n", operadorTemp, opcodeTemp, &tipoTemp) != EOF){ //Percorre a tabela de operadores até o final do arquivo
            if( strncmp(operadorTemp,token,strlen(operadorTemp)) == 0){       // :) se encontrar operação, strcmp retorna 0 e:
                return tipoTemp;                        // Função getTipoOP retorna o tipo da operação [PODE SER: 1, 2 ou 3]
            }
        }
    }
return -1 ;
}


/**Verifica se valor é uma constante**/
int verificaConstante(const char* token){
    if(token[0] == '0' || token[0] == '1' || token[0] == '2' || token[0] == '3' || token[0] == '4' || token[0] == '5'||
       token[0] == '6' || token[0] == '7' || token[0] == '8' || token[0] == '9'){

            if(token[strlen(token)-1]!= ',' && token[strlen(token)-1]!= '.' && token[strlen(token)-1]!= ':')
                return 0;
    }
    return 1;
}

/**ESCREVE NO ARQUIVO DE SAIDA A INSTRUÇÃO EM CÓDIGO OBJETO**/

void geraObjCode(const char *enderecoArqSaida,int endereco,int tipoInstrucao,char label[30],char opcode[20],char operando1[5],char operando2[5],char num_bytes[10],char constante[10]){

    FILE* arquivoSaida;
    arquivoSaida = fopen(enderecoArqSaida,"a+");
    int enderecoLabel = 0,i,troca;
    char binarioOp[6],binarioLabel[11],binarioReg1[11],binarioReg2[11],binarioConstante[11];


    switch(tipoInstrucao)
    {
        case 1:
            if(strcmp(opcode,"return") == 0){
                fprintf(arquivoSaida,"%02hX        :  10000000;\n",endereco);
                fprintf(arquivoSaida,"%02hX        :  00000000;\n",endereco+1);
            ///    printf(" %s \n",opcode);
            }
            if(strcmp(opcode,"stop") == 0){
                fprintf(arquivoSaida,"%02hX        :  00000000;\n",endereco);
                fprintf(arquivoSaida,"%02hX        :  00000000;\n",endereco+1);
           ///     printf(" %s \n",opcode);
            }

        break;

        case 2: /*-----formato de instrução: 5 bits para operação e 11 para endereço----*/

            findOpEnd(opcode,binarioOp);                    // Pega valor operanção em binarioOp

            if(operando1[0] != '\0' && label[0] == '\0'){    // Se label está vazia
                                                            // sabemos que endereço é um registrador
              ///  printf(" %s %s \n",opcode,operando1);
                findEndegistrador(operando1,binarioReg1);

                fprintf(arquivoSaida,"%02hX        :  %s000;\n",endereco,binarioOp);

                fprintf(arquivoSaida,"%02hX        :  ",endereco+1);

                for(i=0; i < (8-strlen(binarioReg1));i++ ){     // completa linha com zeros
                    fprintf(arquivoSaida,"0");
                }
                fprintf(arquivoSaida,"%s;\n",binarioReg1);

            }else{                                          // é uma label
            ///    printf(" %s %s\n",opcode,label);
                enderecoLabel = findLabelEnd(label);
                itoa(enderecoLabel,binarioLabel,2);

                fprintf(arquivoSaida,"%02hX        :  %s000;\n",endereco,binarioOp);

                fprintf(arquivoSaida,"%02hX        :  ",endereco+1);

                for(i=0; i < (8-strlen(binarioLabel));i++ ){
                    fprintf(arquivoSaida,"0");
                }
                fprintf(arquivoSaida,"%s;\n",binarioLabel);
            }

        break;

        case 3: /* Operation form = 5 operação  + 2 destino +9 origem*/
            ///printf("%s\t %d\n",opcode,tipoInstrucao);
            findOpEnd(opcode,binarioOp);
            ///printf("%s\t%s\t%s\n",opcode, operando1,operando2);
            ///printf("%s = %s\n",opcode,binarioOp);
            if(operando1[0] != '\0' && operando2[0] != '\0'){   // operação é entre reg1 e reg2
                ///printf(" %s %s %s\n",opcode,operando1,operando2);

                findEndegistrador(operando2,binarioReg2); ///explicar pq operando 2

                findEndegistrador(operando1,binarioReg1);

                ///printf("%s\t%s\t%s\n",opcode, operando1,operando2);

                fprintf(arquivoSaida,"%02hX        :  %s%s0;\n",endereco,binarioOp,binarioReg1);

                fprintf(arquivoSaida,"%02hX        :  ",endereco+1);
                for(i=0; i < (8-strlen(binarioReg2));i++ ){
                    fprintf(arquivoSaida,"0");
                }
                fprintf(arquivoSaida,"%s;\n",binarioReg2);


            }
            if(operando1[0] != '\0' && label[0] != '\0'){                 // operação é entre reg1 e label
               /// printf(" %s %s %s\n",opcode,operando1,label);

                findEndegistrador(operando1,binarioReg1);
                enderecoLabel = findLabelEnd(label);
                ///printf("%02hX\n\n",endereco);

                itoa(enderecoLabel,binarioLabel,2);


                fprintf(arquivoSaida,"%02hX        :  %s%s0;\n",endereco,binarioOp,binarioReg1);
                fprintf(arquivoSaida,"%02hX        :  ",endereco+1);
                for(i=0; i < (8-strlen(binarioLabel));i++ ){
                    fprintf(arquivoSaida,"0");
                }
                fprintf(arquivoSaida,"%s;\n",binarioLabel);


            }
            if(operando1[0] != '\0' && constante[0] != '\0'){       // operação é entre reg1 e constante
                ///printf(" %s %s %s\n",opcode,operando1,constante);

                findEndegistrador(operando1,binarioReg1);

                troca = atoi(constante);    //converte string em int

                itoa(troca,binarioConstante,2); // converte int em binario
                ///printf("o binario op eh =%s\n o opcode eh=%s\n",binarioOp,opcode);

                fprintf(arquivoSaida,"%02hX        :  %s%s0;\n",endereco,binarioOp,binarioReg1);
                fprintf(arquivoSaida,"%02hX        :  ",endereco+1);

                for(i=0; i < (8-strlen(binarioConstante));i++ ){
                        fprintf(arquivoSaida,"0");
                    }
                fprintf(arquivoSaida,"%s;\n",binarioConstante);
            }

        break;

        case 4: /*caso seja .data*/
           /// printf(" %s %s %s\n",opcode,num_bytes,constante);
            troca = atoi(constante);
            itoa(troca,binarioConstante,2);
            fprintf(arquivoSaida,"%02hX        :  00000000;\n",endereco);
            fprintf(arquivoSaida,"%02hX        :  ",endereco+1);

            for(i=0; i < (8-strlen(binarioConstante));i++ ){
                    fprintf(arquivoSaida,"0");
                }
            fprintf(arquivoSaida,"%s;\n",binarioConstante);
        break;

        default:
            printf("Nao foi possivel construir o object code! \n");
    }

}


/**Registra END no final do arquivo objeto**/
void fim(const char* endArquivo, int endereco){
FILE* arquivo;
//int i;

arquivo = fopen(endArquivo,"a+");
//    for(i=endereco; i<=127;i++){
//        fprintf(arquivo,"%02hX        :  00000000;\n",i);
//    } //OU:::

fprintf(arquivo,"[%02hX..7F]:  00000000;\n",endereco);

fprintf(arquivo,"END;\n"); ///CarlosCorrect

printf("Montagem concluida!\n");
}
