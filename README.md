# sistemas-distribuidos
Sistemas Distribuídos UFRJ 2018.1

# Descrição dos trabalhos

## pub_sub_twitter
Imitação do twitter com modelo baseado em eventos(publish/subscribe em topics) e arquitetura centralizada(um nó despachante).
```
Objetivos
Projetar e implementar uma aplicação que simule o funcionamento do Twitter.
Arquitetura de sistema: 
    Os seguintes componentes deverao ser implementados como programas distintos:
        • Despachante: recebe e registra as inscrições e cancelamentos de seguidores;
        recebe as publicações dos usuários; e entrega as publicações para os seguidores.
        • Pub/Sub: se inscreve como seguidor de um ou mais usuarios; e/ou cadastra-se
        como publicador e publica mensagens com o seu login. Todas essa interações são
        feitas com o despachante.
```

## chat_p2p
Chat p2p + servidor de usuários
```
1. Objetivos:
    Projetar e implementar um chat distribuído.
2. Arquitetura de sistema
    Os seguintes componentes deverão ser implementados como programas distintos:
    • Servidor de usuarios: mantém lista de usuários ativos
    • Cliente de usuários: consulta servidor de usuários e exibe a lista de usuários ativos
    • Par passivo: usuario que espera conexões de outros usuários e mantém diálogos
    • Par ativo: usuario que inicia o diálogo com outro usuário
```

## client-server
Implementação básica cliente-servidor
```
2 versões: servidor sigle-threaded e multithreading que ofereça as seguintes operações:
1. Somar dois números inteiros (positivos ou negativos)
2. Subtrair dois números inteiros (positivos ou negativos)
3. Multiplicar dois números inteiros (positivos ou negativos)
4. Dividir dois números inteiros (positivos ou negativos)
5. Na versão multithread informar quantos clientes estão ativos no servidor
6. Implementar uma aplicação cliente que permita ao usuário interagir com o servidor fazendo chamadas às operações oferecidas
```

## java_RMI
Implementação com middleware de objeto distribuído(JavaRMI)
```
Implementar a calculadora remota (client-server) usando JavaRMI:
Operações
- 1 Somar dois números inteiros (positivos ou negativos)
- 2 Subtrair dois números inteiros (positivos ou negativos)
- 3 Multiplicar dois números inteiros (positivos ou negativos)
- 4 Dividir dois números inteiros (positivos ou negativos)
- 6 Implementar uma aplicação cliente fazendo chamadas às operações oferecidas e experimentar a aplicação em máquinas distintas
```
