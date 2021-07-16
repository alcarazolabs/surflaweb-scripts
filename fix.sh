

#!/usr/bin/env bash
 
#Configure these two variables
MYUSER="freddy"
APACHEGROUP="www-data"
 
 
 
 
SCRIPTPATH=`pwd -P`
BOOTSTRAP="$SCRIPTPATH/bootstrap/"
BOOTSTRAPCACHE="$SCRIPTPATH/bootstrap/cache/"
STORAGE="$SCRIPTPATH/storage"
LOGS="$STORAGE/logs"
 
#add my user to the web server group
sudo usermod -a -G ${APACHEGROUP} ${MYUSER}
 
#make www-data own everything in the directory
sudo chown -R ${MYUSER}:${APACHEGROUP} ${SCRIPTPATH}
 
#change permissions on files to 644
sudo find ${SCRIPTPATH} -type f -exec chmod 0644 {} \;
 
#change permissions on directories to 755
sudo find ${SCRIPTPATH} -type d -exec chmod 0755 {} \;
 
#if i have any bash scripts in there, make them executable
sudo find ${SCRIPTPATH} -type f -iname "*.sh" -exec chmod +x {} \;
 
 
if test ! -d "$BOOTSTRAPCACHE"
then
    MKDIRCOMMAND=`mkdir -p ${BOOTSTRAPCACHE}`
    $MKDIRCOMMAND
fi
 
chown ${MYUSER}:${APACHEGROUP} ${BOOTSTRAP}
 
chown ${MYUSER}:${APACHEGROUP} ${BOOTSTRAPCACHE}
 
chmod 0775 ${BOOTSTRAPCACHE}
 
 
if [ -f ${SCRIPTPATH}/bootstrap/cache/services.php ];
then
    chmod 0664 ${SCRIPTPATH}/bootstrap/cache/services.php
fi
 
if [ ! -d ${SCRIPTPATH}/storage ]; then
    mkdir -p ${SCRIPTPATH}/storage
fi
 
STORAGEFIXCOMMAND=`chown -R  ${MYUSER}:${APACHEGROUP} ${SCRIPTPATH}/storage`
$STORAGEFIXCOMMAND
 
chmod -R 0775 ${STORAGE}

echo 'Permisos establecidos correctamente'
