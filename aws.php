<?php

if ($_POST)
{
    $aws_public_key = $_POST['public'];
    $aws_private_key = $_POST['private'];

    $fileName = $_POST['path'];
    $fileContent = file_get_contents($_FILES['file']['tmp_name']);

    require 'vendor/autoload.php';

    $s3 = new Aws\S3\S3Client([
        'version'     => 'latest',
        'region'      => 'eu-central-1',
        'credentials' => [
            'key'    => $aws_public_key,
            'secret' => $aws_private_key,
        ]
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
