# Extraindo ponteiros
Na pasta `etc/Ponteiros` você encontrará alguns `.bat` numerados para serem executados na ordem indicada.
1. Faz *unpack* do Ragexe e salva na pasta como `unpacked_Ragexe.exe`
**Obs.: se o jogo foi instalado em outro local, abra o `.bat` num editor de texto e edite o caminho na linha a seguir**
	```batch
	SET TARGET_FOLDER=C:\Gravity\Ragnarok
	```
2. Busca pelos ponteiros usando o executável gerado na pasta - os ponteiros serão salvos no arquivo `ponteiros.txt` pronto para colocar no código. **Caso algum ponteiro não seja encontrado, será necessário utilizar ferramentas avançadas como o [IDA](https://hex-rays.com/ida-free) para encontrá-lo.**

## Usando o IDA
Caso a ferramenta na pasta não consiga encontrar algum ponteiro, será necessário recorrer a ferramentas como o IDA, comumente usado para este caso.

A seguir, uma lista de assinaturas/*array of bytes* que a ferramenta usa para buscar os endereços. Verifique no IDA se esses endereços realmente existem:
- **CRAG_CONNECTION_PTR**: `55 8B EC 6A FF 68 ?? ?? ?? ?? 64 A1 00 00 00 00 50 83 EC 18 A1 ?? ?? ?? ?? 33 C5 89 45 F0 53 56 57 50 8D 45 F4 64 A3 00 00 00 00 68 ?? ?? ?? ??`
- **CHECKSUM_FUN_ADDRESS**: `FF B6 84 00 00 00 FF B6 80 00 00 00 50 53 FF 75 0C E8 ?? ?? ?? ?? 8B 4E 74 83 C4 14 88 04 0B`
- **SEED_FUN_ADDRESS**: `8B 4E 74 8B 46 78 2B C1 50 51 E8 ?? ?? ?? ?? 83 C4 08 89 86 80 00 00 00 89 96 84 00 00 00`
- **DOMAIN_ADDRESS_PTR**: `6A FF 68 81 00 00 00 50 E8 ? ? ? ? 83 C4 18 83 3D ? ? ? ? 0D`
- **T_ADDRESS_PTR**: `68 ? ? ? ? 50 6A FF 8D 85 ? ? ? ? 68 81 00 00 00 50 E8 ? ? ? ? 83 C4 14 E8 ? ? ? ? 8B 08 8B 51 04`

**Lembre-se: as assinaturas acima não são absolutas e podem mudar com atualizações do jogo!**