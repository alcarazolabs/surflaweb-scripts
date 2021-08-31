#!/usr/bin/env bash
#source: https://gist.github.com/joelconty/e26a25543a4858435009e143c092e793
#Configure these two variables
MYUSER="joel"
APACHEGROUP="www-data"


#you don't need to edit anything below.

echo "Setting variables..."
SCRIPTPATH=`pwd -P`
BOOTSTRAP="${SCRIPTPATH}/bootstrap/"
BOOTSTRAPCACHE="${SCRIPTPATH}/bootstrap/cache/"
STORAGE="${SCRIPTPATH}/storage"
LOGS="${STORAGE}/logs"
FRAMEWORK="${STORAGE}/framework"


#add my user to the web server group
echo "adding ${MYUSER} to the ${APACHEGROUP} group ..."
sudo usermod -a -G ${APACHEGROUP} ${MYUSER}



#if bootstrap/cache does not exist, create it
if [ ! -d ${BOOTSTRAPCACHE} ]; then
    echo "Creating ${BOOTSTRAPCACHE} directory..."
    mkdir -p ${BOOTSTRAPCACHE}
fi

#if we don't have a .gitignore file in bootstrap/cache, add it
if [ ! -f ${BOOTSTRAPCACHE}/.gitignore ]; then
    echo "Creating .gitignore file in ${BOOTSTRAPCACHE}/..."
    echo '*' > ${BOOTSTRAPCACHE}/.gitignore
    echo '!.gitignore' >> ${BOOTSTRAPCACHE}/.gitignore
fi


#if storage dir does not exist, create it.
if [ ! -d ${STORAGE} ]; then
    echo "Creating ${STORAGE} directory"
    mkdir -p ${STORAGE}
fi


#then, make sure we have debugbar, framework and logs inside storage
#debugbar is not installed by default, but it does not hurt to have it's directory there , ready for when you install it
for DIR in debugbar framework logs
do
    if [ ! -d ${STORAGE}/${DIR} ]; then
        echo "Creating ${STORAGE}/${DIR} directory..."
        mkdir -p ${STORAGE}/${DIR}
    else
        echo "Checking for existance of  ${STORAGE}/${DIR}...  It exists"
    fi
done

#if we don't have a .gitignore file in storage/framework, add it
if [ ! -f ${FRAMEWORK}/.gitignore ]; then
    echo "Creating .gitignore file in ${FRAMEWORK}..."
    echo 'compiled.php' > ${FRAMEWORK}/.gitignore
    echo 'config.php' >> ${FRAMEWORK}/.gitignore
    echo 'down' >> ${FRAMEWORK}/.gitignore
    echo 'events.scanned.php' >> ${FRAMEWORK}/.gitignore
    echo 'maintenance.php' >> ${FRAMEWORK}/.gitignore
    echo 'routes.php' >> ${FRAMEWORK}/.gitignore
    echo 'routes.scanned.php' >> ${FRAMEWORK}/.gitignore
    echo 'schedule-*' >> ${FRAMEWORK}/.gitignore
    echo 'services.json' >> ${FRAMEWORK}/.gitignore
fi


#then, make sure we have  cache, sessions, testing and views inside storage/framework
for DIR in cache sessions testing views
do
    if [ ! -d ${FRAMEWORK}/${DIR} ]; then
        echo "Creating ${FRAMEWORK}/${DIR} directory..."
        mkdir -p ${FRAMEWORK}/${DIR}
    else
        echo "Checking for existance of  ${FRAMEWORK}/${DIR}...  It exists"
    fi
done


#then, make sure we have a storage/framework/cache/data directory
if [ ! -d ${FRAMEWORK}/cache/data ]; then
    echo "Creating ${FRAMEWORK}/cache/data directory..."
    mkdir -p ${FRAMEWORK}/cache/data
else
    echo "Checking for existance of  ${FRAMEWORK}/cache/data...  It exists"
fi



#if we don't have a .gitignore file in storage/framework/cache/data, add it
if [ ! -f ${FRAMEWORK}/cache/data/.gitignore ]; then
    echo "Creating .gitignore file in ${FRAMEWORK}/cache/data/..."
    echo '*' > ${FRAMEWORK}/cache/data/.gitignore
    echo '!data/' >> ${FRAMEWORK}/cache/data/.gitignore
    echo '!.gitignore' >> ${FRAMEWORK}/cache/data/.gitignore
fi

#if we don't have a .gitignore file in storage/framework/sessions/, add it
if [ ! -f ${FRAMEWORK}/sessions/.gitignore ]; then
    echo "Creating .gitignore file in ${FRAMEWORK}/sessions/..."
    echo '*' > ${FRAMEWORK}/sessions/.gitignore
    echo '!.gitignore' >> ${FRAMEWORK}/sessions/.gitignore
fi

#if we don't have a .gitignore file in storage/framework/testing/, add it
if [ ! -f ${FRAMEWORK}/testing/.gitignore ]; then
    echo "Creating .gitignore file in ${FRAMEWORK}/testing/..."
    echo '*' > ${FRAMEWORK}/testing/.gitignore
    echo '!.gitignore' >> ${FRAMEWORK}/testing/.gitignore
fi

#if we don't have a .gitignore file in storage/framework/views/, add it
if [ ! -f ${FRAMEWORK}/views/.gitignore ]; then
    echo "Creating .gitignore file in ${FRAMEWORK}/views/..."
    echo '*' > ${FRAMEWORK}/views/.gitignore
    echo '!.gitignore' >> ${FRAMEWORK}/views/.gitignore
fi


#make my user and the apache group  own everything in the directory
echo "Fixing ownership on everything..."
sudo chown -R ${MYUSER}:${APACHEGROUP} ${SCRIPTPATH}

#change permissions on files to 644
echo "Setting permissions 0644 on all files... (excluding vendor, node_modules and .git directories)"
sudo find ${SCRIPTPATH} -type f -not -path "${SCRIPTPATH}/vendor/*" -not -path "${SCRIPTPATH}/node_modules/*"  -not -path "${SCRIPTPATH}/.git/*" -exec chmod 0644 {} \;


#change permissions on directories to 755
echo "Setting permissions 0755 on all directories... (excluding vendor, node_modules and .git directories)"
sudo find ${SCRIPTPATH} -type d -not -path "${SCRIPTPATH}/vendor/*" -not -path "${SCRIPTPATH}/node_modules/*"  -not -path "${SCRIPTPATH}/.git/*" -exec chmod 0755 {} \;

#change permissions on bootstrap/cache to 775
echo "Setting permissions 0775 on bootstrap/cache"
sudo chmod 0775 ${BOOTSTRAPCACHE}

#change permissions on the php files that live in bootstrap/cache to 0775
echo "Setting permissions 0775 on php files in bootstrap/cache"
sudo chmod 0775 ${BOOTSTRAPCACHE}/*.php

#if i have any bash scripts in there, make them executable
echo "Making any existing shell scripts executable"
sudo find ${SCRIPTPATH} -type f -iname "*.sh" -exec chmod +x {} \;

#finally fix permissions on storage
echo "Setting 0775 permissions on ${STORAGE}..."
chmod -R 0775 ${STORAGE}