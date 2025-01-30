# Reconhecimento Facial em C++ com OpenCV

Este repositório contém um **sistema completo de cadastramento e reconhecimento facial** escrito em C++ utilizando a biblioteca [OpenCV](https://opencv.org/) e modelos de Deep Learning (DNN). O objetivo é fornecer uma **base sólida** para projetos de biometria e segurança, permitindo o cadastro de usuários, extração de embeddings faciais e verificação de identidade em tempo real.

---
## Sumário
1. [🎯 Visão Geral do Projeto](#visao-geral-do-projeto)
2. [✨ Recursos Principais](#recursos-principais)
3. [📁 Estrutura de Pastas](#estrutura-de-pastas)
4. [⚙️ Dependências](#dependencias)
5. [🔧 Como Compilar](#como-compilar)
6. [🚀 Como Executar](#como-executar)
7. [➡️ Fluxo de Uso](#fluxo-de-uso)
8. [❗ Observações Importantes](#observacoes-importantes)
9. [🤝 Contribuindo](#contribuindo)
10. [📝 Licença](#licenca)
11. [👤 Autor](#autor)

---

## <a name="visao-geral-do-projeto"></a>🎯 Visão Geral do Projeto
Este sistema permite:
- **Cadastrar** novos usuários, armazenando seus embeddings faciais (usando redes neurais para extração de características).
- **Reconhecer** ou verificar a identidade de um usuário em tempo real via webcam.
- **Gerar logs** de ações (log de erros e log de informações).

Ele é construído em C++ com **OpenCV** (tanto para detecção de rostos quanto para extração de embeddings) e faz uso de **modelos DNN pré-treinados**, como a SSD (Single Shot Detector) para detecção facial e a rede [OpenFace](https://github.com/cmusatyalab/openface) para geração de embeddings.

Repositório completo:  
[https://github.com/GustavoDiego/Reconhecimento-Facial](https://github.com/GustavoDiego/Reconhecimento-Facial)

---

## <a name="recursos-principais"></a>✨ Recursos Principais
- **Cadastro de Pessoas** com múltiplas fotos: o programa captura diversas imagens do rosto para criar um **embedding médio** mais robusto.
- **Detecção Facial via DNN**: utilização da rede [Res10 SSD](https://github.com/opencv/opencv/tree/master/samples/dnn/face_detector) para detectar rostos.
- **Extração de Embeddings** via modelo OpenFace (`.t7`).
- **Comparação de Similaridade**: cálculo de similaridade de cosseno para verificar se o rosto corresponde a um determinado usuário.
- **Logs de Erro e Informação**: cada ação relevante é registrada em arquivos de log distintos.

---

## <a name="estrutura-de-pastas"></a>📁 Estrutura de Pastas
A estrutura principal do projeto (conforme o *print* compartilhado) se organiza da seguinte forma:

```
Reconhecimento-Facial/
│
├── faces_cadastradas/   # Pasta para armazenar rostos ou recursos adicionais
├── models/              # Modelos DNN 
│
├── .gitignore           # Lista de arquivos/pastas ignorados pelo Git
├── database.cpp         # Implementação das funções de banco de dados
├── database.h           # Cabeçalho para banco de dados
│
├── face_recognition.cpp # Funções de reconhecimento/verificação facial
├── face_recognition.h
├── face_registration.cpp# Funções de cadastro de pessoas
├── face_registration.h
│
├── main.cpp             # Ponto de entrada do programa (menu principal)
│
├── OPENCVTESTE3.vcxproj        # Arquivo de projeto do Visual Studio
│
├── utils.cpp            # Funções auxiliares (detecção, extração de embeddings, logging)
└── utils.h
```

---

## <a name="dependencias"></a>⚙️ Dependências
1. **OpenCV** (versão 4.x recomendada):
   - Necessário para operações de detecção de rostos, manipulação de imagens e DNN.
2. **Compilador C++** compatível (Visual Studio, MinGW, etc.).
3. **Modelos de Rede**:
   - `deploy.prototxt` e `res10_300x300_ssd_iter_140000.caffemodel` (detecção facial).
   - `openface.nn4.small2.v1.t7` (extração de embeddings).
4. **CMake** (opcional) se preferir compilar multiplataforma. Se usar Visual Studio, basta abrir o `.sln/.vcxproj`.

---

## <a name="como-compilar"></a>🔧 Como Compilar

### Visual Studio (Windows)
1. **Clone** este repositório ou faça o download.
2. **Abra** o arquivo `OPENCVTESTE3.vcxproj` ou a solução correspondente no Visual Studio.
3. **Configure**:
   - Se necessário, ajuste o **include path** e **library path** do OpenCV.
   - Certifique-se de que o projeto esteja configurado para **x64**, se for o seu caso.
4. Clique em **Build** / **Compilar**.

### Linha de Comando (CMake/Multiplataforma)
1. Assegure-se de ter [CMake](https://cmake.org/) instalado.
2. Crie uma pasta de build:
   ```bash
   mkdir build && cd build
   ```
3. Gere os arquivos de build e compile:
   ```bash
   cmake ..
   cmake --build .
   ```
4. O executável final será criado em `build/`.

---

## <a name="como-executar"></a>🚀 Como Executar
1. **Localize o executável** (por exemplo, `Reconhecimento-Facial.exe`) gerado pela compilação no modo Release ou Debug.
2. **Certifique-se** de que a **pasta `models/`** esteja ao lado do executável, contendo:
   - `deploy.prototxt`
   - `res10_300x300_ssd_iter_140000.caffemodel`
   - `openface.nn4.small2.v1.t7`
3. **Execute** o programa:
   ```bash
   ./Reconhecimento-Facial.exe
   ```
4. Um **menu** será exibido no terminal/console, onde você pode **cadastrar pessoas**, **reconhecer/verificar** ou **sair**.

---

## <a name="fluxo-de-uso"></a>➡️ Fluxo de Uso
1. **Abrir o programa** e escolher a opção **[1] Cadastrar nova pessoa**.
   - O sistema pedirá o nome do usuário.
   - Capturará múltiplas fotos (pressionando `s` para cada captura) para gerar um **embedding médio**.
   - Salvará o embedding resultante no arquivo `.xml` (por padrão).
2. **Voltar ao menu** e escolher **[2] Reconhecer pessoa** ou **VerificarIdentidade()**.
   - Solicita o nome ou simplesmente exibe quem está sendo detectado.
   - Faz a comparação com embeddings armazenados.
   - Retorna o resultado no console/viewport da câmera.
3. **[3] Sair**: encerra o programa.

---

## <a name="observacoes-importantes"></a>❗ Observações Importantes
- **Banco de Dados Local**: Embeddings são salvos em arquivos `.xml` (nome de usuário + `.xml`) e listados em `database_config.txt` (caso não esteja ignorado).  
- **Limiar de Similaridade**: Ajuste das constantes de similaridade para decidir se um rosto é “conhecido” ou “desconhecido” (por exemplo, 0.6 ou 0.65) pode variar conforme a aplicação.
- **Logs**:  
  - `log_info` contém registros gerais de sucesso ou ações importantes.  
  - `log_error` armazena erros, como falhas de detecção, problemas de I/O etc.
- **Pasta `x64`**: Não precisa ser versionada, pois contém artefatos de build gerados pelo Visual Studio (listada em `.gitignore`).

---

## <a name="contribuindo"></a>🤝 Contribuindo
1. **Fork** este repositório.
2. Crie uma branch de feature: `git checkout -b minha-nova-feature`.
3. Faça **commit** das suas mudanças: `git commit -m 'Adiciona nova funcionalidade X'`.
4. Faça **push** para a branch: `git push origin minha-nova-feature`.
5. **Abra um Pull Request** explicando suas alterações.

---

## <a name="licenca"></a>📝 Licença
Este projeto está licenciado sob a **MIT License**.  
Sinta-se à vontade para usar, modificar e distribuir com poucas restrições.

---

## <a name="autor"></a>👤 Autor
- **GustavoDiego**  
  - [GitHub](https://github.com/GustavoDiego)  

Sinta-se à vontade para abrir *Issues* em caso de dúvidas, sugestões ou problemas!  