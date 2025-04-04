process pre1 {
    // Executa o processo dentro do container 'pre1docker'
    container 'pre1docker'
    
    // Usa o modo 'link' para operar diretamente sobre os arquivos originais
    stageInMode 'link'
    
    // Ao final, publica o arquivo modificado na pasta "data", sobrescrevendo o arquivo original
    publishDir 'data', mode: 'copy', overwrite: true
    
    input:
        // Recebe o arquivo do canal e o atribui à variável 'infile'
        file infile

    output:
        // Retorna o mesmo arquivo (já modificado)
        file infile

    script:
    """
    echo "Processando arquivo: \$(basename ${infile})"
    # Converte o arquivo para o formato Unix sobrescrevendo o arquivo original
    dos2unix ${infile}
    echo "Arquivo sobrescrito e convertido para formato Unix: \$(basename ${infile})"
    """
}
