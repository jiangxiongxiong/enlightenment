#! /bin/sh

DB="edb_ed $1 add"
STR="str"

$DB "*.png"     $STR "image/png"
$DB "*.gif"     $STR "image/gif"
$DB "*.jpeg"    $STR "image/jpeg"
$DB "*.jpg"     $STR "image/jpeg"
$DB "*.ppm"     $STR "image/ppm"
$DB "*.pgm"     $STR "image/pgm"
$DB "*.pbm"     $STR "image/pbm"
$DB "*.pnm"     $STR "image/pnm"
$DB "*.rgba"    $STR "image/rgba"
$DB "*.icon"    $STR "image/png"
$DB "*.xcf"     $STR "image/xcf"
$DB "*.bmp"     $STR "image/bmp"
$DB "*.xpm"     $STR "image/xpm"
$DB "*.blend"   $STR "image/blender"

$DB "*.ttf"     $STR "font/true-type"

$DB "*.tar"     $STR "application/tar"
$DB "*.tar.gz"  $STR "application/tar-gzip"
$DB "*.gz"      $STR "application/gnu-zip"
$DB "*.tgz"     $STR "application/tar-gzip"
$DB "*.tar.Z"   $STR "application/tar-compressed"
$DB "*.tar.bz2" $STR "application/tar-bzip2"
$DB "*.bz2"     $STR "application/bzip2"
$DB "*.zip"     $STR "application/zip"

$DB "*.rpm"     $STR "archive/rpm"
$DB "*.deb"     $STR "archive/debian"

$DB "*.mp3"     $STR "audio/mp3"
$DB "*.m3u"     $STR "audio/mp3"
$DB "*.wav"     $STR "audio/wav"
$DB "*.mod"     $STR "audio/mod"
$DB "*.s3m"     $STR "audio/s3m"
$DB "*.xm"      $STR "audio/xm"

$DB "*.mpeg"    $STR "video/mpeg"
$DB "*.mpg"     $STR "video/mpeg"
$DB "*.avi"     $STR "video/x-msvideo"
$DB "*.mov"     $STR "video/quicktime"

$DB "*.html"    $STR "text/html"
$DB "*.htm"     $STR "text/html"
$DB "*.txt"     $STR "text/plain"
$DB "README"    $STR "text/readme"
$DB "INSTALL"   $STR "text/install"
$DB "COPYING"   $STR "text/copyright"
$DB "NEWS"      $STR "text/news"
$DB "TODO"      $STR "text/todo"
$DB "configure" $STR "text/configure"
$DB "*.c"       $STR "text/c"
$DB "*.cc"      $STR "text/c++"
$DB "*.cpp"     $STR "text/c++"
$DB "*.c++"     $STR "text/c++"
$DB "*.h"       $STR "text/h"
$DB "*.h++"     $STR "text/h"
$DB "Makefile"  $STR "text/makefile"
$DB "*.o"       $STR "binary/object"
$DB "core"      $STR "binary/core"
$DB "*~"        $STR "document/backup"

$DB "*.epplet"  $STR "application/enlightenment-epplet"
