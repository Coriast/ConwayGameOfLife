# John Conway's Game Of Life

## Sobre
O jogo da vida é um autômato que simula o comportamento de células baseado nas regras:
1. Qualquer célula viva que tenha entre dois e três vizinhos permanece viva.
2. Qualquer célula morta com exatamente três vizinhos vivos volta a vida.
3. Qualquer outra célula morre na próxima geração.

## Projeto

**A** -> Para iniciar as gerações

**I** -> Para retornar para o estado Idle

Controles gerais de Input | Window e afins pelo SFML 

Para renderização do GRID está sendo usado OpenGL 3

A lógica para o grid "infinito" segue o documento [Rokicki, T., 2018. Life Algorithms.](https://www.gathering4gardner.org/g4g13gift/math/RokickiTomas-GiftExchange-LifeAlgorithms-G4G13.pdf)

A implementação utilizada foi da Lista simples de células.

![image](https://imgur.com/Av3Sv59.gif)
