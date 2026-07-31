# Próxima Data em Assembly MIPS

Implementação de uma função em **Assembly MIPS** que recebe, por referência, três números inteiros representando dia, mês e ano, e modifica esses valores para representar a data do dia seguinte.

O projeto foi desenvolvido na disciplina de **Organização e Arquitetura de Computadores I** como uma aplicação prática de conceitos de arquitetura MIPS, como manipulação de registradores, acesso à memória, desvios, repetição, sub-rotinas e uso da pilha.

## Objetivo

A implementação em Assembly corresponde à seguinte função em linguagem C:

```c
void proximo_dia(int *dia, int *mes, int *ano);
```

A função recebe os endereços das três variáveis e atualiza diretamente seus valores na memória.

A solução trata:

- incremento de dias comuns;
- mudança de mês;
- mudança de ano;
- quantidade específica de dias de cada mês;
- fevereiro em anos bissextos.

> A implementação pressupõe que a data de entrada é válida. Não há uma etapa específica de validação de datas inválidas.

## Estrutura do repositório

```text
.
├── proximo_dia.asm   # Implementação em Assembly MIPS
├── proximo_dia.c     # Solução equivalente em linguagem C
└── README.md         # Documentação do projeto
```

## Funcionamento do algoritmo

A quantidade padrão de dias de cada mês é armazenada em uma tabela:

```asm
dias_mes:
    .word 31,28,31,30,31,30,31,31,30,31,30,31
```

O algoritmo segue estas etapas:

1. Carrega os valores atuais de dia, mês e ano.
2. Percorre a tabela `dias_mes` até encontrar a posição correspondente ao mês.
3. Verifica se o mês é fevereiro.
4. Caso seja fevereiro, chama a sub-rotina `bissexto`.
5. Incrementa o dia.
6. Se o dia ultrapassar o limite do mês, redefine o dia para `1` e incrementa o mês.
7. Se o mês ultrapassar `12`, redefine o mês para `1` e incrementa o ano.
8. Armazena os valores atualizados nos endereços recebidos.

## Organização do código Assembly

O arquivo Assembly possui três partes principais:

### Seção de dados

A diretiva `.data` contém:

- as variáveis `dia`, `mes` e `ano`, utilizadas no caso de teste;
- a tabela `dias_mes`, com a quantidade padrão de dias de cada mês.

```asm
.data
    dia: .word 28
    mes: .word 2
    ano: .word 2024

    dias_mes:
        .word 31,28,31,30,31,30,31,31,30,31,30,31
```

### `main`

A função `main`:

- carrega os endereços de `dia`, `mes` e `ano` nos registradores de argumento;
- chama `proximo_dia` com `jal`;
- imprime a data atualizada;
- encerra o programa por meio de uma chamada de sistema.

```asm
la  $a0, dia
la  $a1, mes
la  $a2, ano

jal proximo_dia
```

### Sub-rotinas

O programa possui duas sub-rotinas:

| Sub-rotina    | Responsabilidade                                         |
| ------------- | -------------------------------------------------------- |
| `proximo_dia` | Calcula a data seguinte e atualiza os valores na memória |
| `bissexto`    | Retorna se o ano informado é bissexto                    |

## Sub-rotina `proximo_dia`

### Parâmetros

| Registrador | Conteúdo                   |
| ----------- | -------------------------- |
| `$a0`       | Endereço da variável `dia` |
| `$a1`       | Endereço da variável `mes` |
| `$a2`       | Endereço da variável `ano` |

Os parâmetros são endereços, permitindo que a sub-rotina modifique diretamente os valores originais por meio das instruções `lw` e `sw`.

### Uso da pilha

Antes de chamar outras sub-rotinas, `proximo_dia` reserva 16 bytes na pilha:

```asm
addi $sp, $sp, -16
sw   $ra, 12($sp)
sw   $a0, 8($sp)
sw   $a1, 4($sp)
sw   $a2, 0($sp)
```

São preservados:

- o endereço de retorno armazenado em `$ra`;
- o endereço de `dia`;
- o endereço de `mes`;
- o endereço de `ano`.

Isso é necessário porque `$a0` é reutilizado para enviar o ano à sub-rotina `bissexto`, enquanto `jal` altera o conteúdo de `$ra`.

Ao final, o endereço de retorno é restaurado e o espaço da pilha é liberado:

```asm
lw   $ra, 12($sp)
addi $sp, $sp, 16
jr   $ra
```

### Busca do mês

A posição correspondente ao mês é localizada por meio de um laço.

```asm
la   $t3, dias_mes
li   $t4, 1

loop:
    beq  $t4, $t1, fim_loop
    addi $t3, $t3, 4
    addi $t4, $t4, 1
    j    loop
```

O registrador `$t3` começa apontando para o primeiro elemento da tabela. A cada repetição, seu endereço avança 4 bytes, tamanho de uma palavra em MIPS, até alcançar o mês desejado.

Após o laço, a quantidade de dias é carregada:

```asm
lw $t5, 0($t3)
```

### Atualização da data

O dia é incrementado:

```asm
addi $t0, $t0, 1
```

A instrução `slt` verifica se o novo dia ultrapassou o limite do mês:

```asm
slt $t6, $t5, $t0
```

Quando isso ocorre, o dia volta para `1` e o mês é incrementado. Se o mês ultrapassar `12`, o mês também volta para `1` e o ano é incrementado.

## Sub-rotina `bissexto`

### Parâmetro e retorno

| Registrador | Conteúdo                                      |
| ----------- | --------------------------------------------- |
| `$a0`       | Ano a ser verificado                          |
| `$v0`       | `1` se o ano for bissexto; `0` caso contrário |

A regra utilizada é:

- anos divisíveis por `400` são bissextos;
- anos divisíveis por `100`, mas não por `400`, não são bissextos;
- anos divisíveis por `4`, mas não por `100`, são bissextos;
- os demais anos não são bissextos.

A implementação utiliza `div` e `mfhi` para obter o resto das divisões.

```asm
li   $t0, 4
div  $a0, $t0
mfhi $t1
```

Quando o resto é zero, o ano é divisível pelo valor testado.

## Registradores utilizados

| Registrador | Utilização                                             |
| ----------- | ------------------------------------------------------ |
| `$a0`–`$a2` | Passagem de parâmetros                                 |
| `$v0`       | Retorno de `bissexto` e seleção de chamadas de sistema |
| `$t0`       | Dia ou divisor utilizado em `bissexto`                 |
| `$t1`       | Mês ou resto de uma divisão                            |
| `$t2`       | Ano                                                    |
| `$t3`       | Endereço atual na tabela `dias_mes`                    |
| `$t4`       | Contador utilizado na busca do mês                     |
| `$t5`       | Limite de dias do mês                                  |
| `$t6`–`$t7` | Resultados temporários de comparações                  |
| `$sp`       | Ponteiro da pilha                                      |
| `$ra`       | Endereço de retorno das sub-rotinas                    |

## Instruções e recursos MIPS empregados

A solução utiliza:

- `la` para carregar endereços;
- `li` para carregar constantes;
- `lw` e `sw` para leitura e escrita na memória;
- `addi` para incrementos e manipulação da pilha;
- `slt` para comparações;
- `beq` e `bne` para desvios condicionais;
- `j` para o laço de repetição;
- `jal` para chamadas de sub-rotina;
- `jr` para retorno;
- `div` e `mfhi` para verificar divisibilidade;
- `move` para preparar o argumento de `bissexto`;
- `syscall` para impressão e encerramento.

As instruções `la`, `li` e `move` são pseudo-instruções aceitas pelo MARS.

## Correspondência com a solução em C

A versão em C utiliza a mesma organização lógica da implementação Assembly:

```c
int limite = dias_mes[*mes - 1];

if (*mes == 2 && bissexto(*ano))
    limite = 29;

(*dia)++;
```

A expressão `dias_mes[*mes - 1]`, direta em C, é implementada no Assembly por meio do laço que percorre a tabela em incrementos de 4 bytes.

Da mesma forma, os ponteiros `int *dia`, `int *mes` e `int *ano` são representados pelos endereços recebidos em `$a0`, `$a1` e `$a2`.

## Como executar o Assembly

O programa foi desenvolvido para o simulador **MARS**.

### Requisitos

- Java instalado;
- simulador MARS;
- arquivo `proximo_dia.asm`.

### Interface gráfica

1. Abra o MARS:

```bash
java -jar <arquivo-do-mars>.jar
```

2. Selecione `File → Open`.
3. Abra `proximo_dia.asm`.
4. Monte o programa com `Run → Assemble` ou `F3`.
5. Execute com `Run → Go` ou `F5`.
6. Consulte o resultado na aba `Run I/O`.

### Linha de comando

```bash
java -jar <arquivo-do-mars>.jar nc proximo_dia.asm
```

Exemplo:

```bash
java -jar Mars4_5.jar nc proximo_dia.asm
```

O nome do arquivo `.jar` pode variar conforme a versão instalada.

## Alteração da data de teste

Para testar outra data no Assembly, altere os valores da seção `.data`:

```asm
dia: .word 28
mes: .word 2
ano: .word 2024
```

Na solução em C, altere as variáveis declaradas em `main`:

```c
int dia = 28;
int mes = 2;
int ano = 2024;
```

## Casos de teste

| Entrada      | Saída esperada | Situação                      |
| ------------ | -------------- | ----------------------------- |
| `15/06/2024` | `16/06/2024`   | Incremento comum              |
| `30/04/2024` | `01/05/2024`   | Mudança de mês                |
| `28/02/2023` | `01/03/2023`   | Fevereiro em ano não bissexto |
| `28/02/2024` | `29/02/2024`   | Fevereiro em ano bissexto     |
| `29/02/2024` | `01/03/2024`   | Fim de fevereiro bissexto     |
| `31/12/2023` | `01/01/2024`   | Mudança de ano                |

## Exemplo de saída

Para a entrada:

```text
28/2/2024
```

o programa imprime:

```text
29/2/2024
```

## Limitações

- A data deve ser alterada diretamente no código antes da execução.
- O programa não recebe valores pelo terminal.
- A implementação pressupõe uma data de entrada válida.

## Uso acadêmico

Este repositório possui finalidade educacional e de portfólio.

Materiais institucionais da disciplina, como enunciados, documentos de avaliação, apresentações e conteúdos disponibilizados em ambiente acadêmico, não fazem parte do repositório.

Os arquivos publicados representam exclusivamente a solução desenvolvida.
