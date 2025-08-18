$docsPath = "c:\Users\benam\source\cpp\Diablo 2 Map Editor\docs"
$imageReplacements = @{
    './DS1 Editor_files/' = './images/'
    './Diablo II DS1 (map) Editor_files/' = './images/'
    './Ds1 Editor - Tutorial 1_files/' = './images/'
    './Adding ANY Monsters and ANY Objects to a DS1_files/' = './images/'
    'DS1 Editor_files/' = 'images/'
    'Diablo II DS1 (map) Editor_files/' = 'images/'
    'Ds1 Editor - Tutorial 1_files/' = 'images/'
    'Adding ANY Monsters and ANY Objects to a DS1_files/' = 'images/'
}

Get-ChildItem -Path $docsPath -Filter "*.html" | ForEach-Object {
    Write-Host "Processing $($_.Name)"
    $content = Get-Content $_.FullName -Raw
    
    $originalContent = $content
    foreach ($oldPath in $imageReplacements.Keys) {
        $newPath = $imageReplacements[$oldPath]
        $content = $content -replace [regex]::Escape($oldPath), $newPath
    }
    
    if ($content -ne $originalContent) {
        Set-Content -Path $_.FullName -Value $content -NoNewline
        Write-Host "Updated $($_.Name)"
    } else {
        Write-Host "No changes needed for $($_.Name)"
    }
}

Write-Host "Image path updates completed!"
