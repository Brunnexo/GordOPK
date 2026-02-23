> [!CAUTION]
> Após a primeira atualização de fevereiro de 2026, as ferramentas presentes nesse repositório **deixaram de funcionar!**<br>
> O repositório permanecerá disponível para fins de estudo e aplicações derivadas.<br><br>
> **O código-fonte presente neste repositório não refletirá mais nas soluções futuras para o OpenKore!**<br>
> **Não insista!**
---
![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white) ![Visual Studio](https://img.shields.io/badge/Visual%20Studio-5C2D91.svg?style=for-the-badge&logo=visual-studio&logoColor=white) [![Discord](https://img.shields.io/badge/Discord-%235865F2.svg?style=for-the-badge&logo=discord&logoColor=white)](https://discord.gg/HyJjHK5zB2) 
# GordOPK
*DLL injetável **simples** para conectar o OpenKore ao cliente Ragnarök Online LATAM.*

Este código é uma versão **muito** reduzida e **muito** comentada para fins de estudo: não possui interface gráfica, notificações nem outros recursos que fogem do conceito da simplicidade.
É só **compilar e rodar!**
Você só precisa garantir que os endereços estejam de acordo com a versão do `Ragexe.exe` que você possui.

Dúvidas? **Acesse o [Discord da comunidade!](https://discord.gg/HyJjHK5zB2)**

## Dependências
### OpenKore
[**É recomendável utilizar uma instalação limpa do OpenKore!**](etc/opk/readme.md)
### Ferramentas
Todas as dependências necessárias para compilar e extrair endereços estão na pasta [`etc/deps`](etc/deps).
Use os scripts para **instalar o necessário** para compilar e executar as ferramentas.
**Nota: instale o [GIT](https://git-scm.com/) para evitar problemas!**

## Compilando
- Verifique se as **ferramentas foram instaladas**
- Execute o `build.bat` da raiz do repositório: será gerado o `GordOPK.asi` na mesma pasta
- Copie o `GordOPK.asi` para a pasta raiz do jogo
- Inicie o OpenKore e o jogo

## Extraindo endereços/ponteiros
A extração de endereços essenciais para a DLL pode ser feita de diversas maneiras.
[Aqui você pode encontrar ferramentas que podem te ajudar com isso!](etc/ptr/readme.md)

## Contribua e compartilhe!

Ajude a democratizar o acesso **gratuito** ao OpenKore, **como tem que ser**!

## Obrigado pelas estrelas!!
<a href="https://www.star-history.com/#Brunnexo/GordOPK&type=date&legend=bottom-right">
 <picture>
   <source media="(prefers-color-scheme: dark)" srcset="https://api.star-history.com/svg?repos=Brunnexo/GordOPK&type=date&theme=dark&legend=bottom-right" />
   <source media="(prefers-color-scheme: light)" srcset="https://api.star-history.com/svg?repos=Brunnexo/GordOPK&type=date&legend=bottom-right" />
   <img alt="Star History Chart" src="https://api.star-history.com/svg?repos=Brunnexo/GordOPK&type=date&legend=bottom-right" />
 </picture>
</a>

[**Estrelas motivam! ⭐**](https://www.star-history.com/#Brunnexo/GordOPK&type=date&legend=bottom-right)
