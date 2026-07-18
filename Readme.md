# Como rodar os dicionários em sua máquina

Para rodar os códigos desse projeto, é necessário seguir algumas intruções

## Instalações necessárias

```bash
sudo apt-get install libicu-dev
```

Para rodar os comandos da biblioteca ICU

## Rodar freq para ler os livros

Use no terminal:

```bash
./freq <avl/rbt/hash/openhash> book.txt
```

Na pasta `scripts` para alguma das estruturas

Caso dê algum erro, digite no terminal:
```bash
chmod +x nome_do_script.sh
```

Se for feita alguma mudança nas implementações, rode na pasta da implementação:

```bash
g++ -std=c++17 -Wall -Wextra main.cpp -o freq_<avl/rbt/hash/openhash> -licuuc -licui18n -licuio
```

e mova o executável criado para a pasta `bin` e substitua o arquivo existente
