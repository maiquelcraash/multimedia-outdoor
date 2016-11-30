Trabalho Final Microprocessador Avançados - UNIVATES - 2016B

Como Configurar e Rodar?

 - Dentro da pasta "slides" deve conter 1 pasta para cada slide a ser exibido. 
    - Cada slide, deverá conter uma imagem chamada "img" (formato png, jpg max 1,5MB)
    - Cada slide, deverá conter um arquivo "text.txt" onde a 1ª linha deve ser o título, a 2ª deve ser o subtítulo e a 3ª, a cor na qual o texto deve aparecer
 - Configurar no arquivo "slides/config.properties" as mensagens e a cidade a ser utilizada na informação do clima
 - Compilar o arquivo server.c
    - $ gcc -o server server.c
 - Rodar o server com a porta 3001
    - ./server 3001
 - Abrir o arquivo index.html no firefox ou chrome