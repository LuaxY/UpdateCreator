<?php

if (@$_POST['key'] == "zad7fdhopjkb5s4za8e4fs")
{
    $fileName = $_FILES['file']['name'];
    $fileContent = file_get_contents($_FILES['file']['tmp_name']);

    require 'vendor/autoload.php';

    $s3 = new Aws\S3\S3Client([
        'version' => 'latest',
        'region'  => 'eu-central-1'
    ]);

    $r = $s3->putObject([
        'Bucket' => 'arkalys',
        'Key'    => $_POST['version'] . '/' . $fileName,
        'Body'   => $fileContent,
        'ACL'    => 'public-read',
    ]);

    echo "OK";
}

?>

<form method="POST" enctype="multipart/form-data">
    Version : <input type="text" name="version" value="1"><br>
    Key : <input type="text" name="key" value="zad7fdhopjkb5s4za8e4fs"><br>
    File : <input type="file" name="file"><br>
    <input type="submit">
</form>