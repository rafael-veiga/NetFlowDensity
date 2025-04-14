process pre1 {
    container 'pre1docker'
    
    input:
        path infile

    output:
        path '*C.csv'

    script:
    """
    filename=\$(basename "${infile}")
    name=\${filename%.*}
    ext=\${filename##*.}
    cp "\$filename" "\${name}C.\${ext}"
    echo "\${name}C.\${ext}"
    dos2unix "\${name}C.\${ext}"
    """
}
