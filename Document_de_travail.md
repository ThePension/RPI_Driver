# 09.05.22

## setup git

cd ~/LinEmb/projects
git clone https://gitlab-etu.ing.he-arc.ch/isc/2021-22/niveau-2/2247.1-linux-emb-il/gr9.git

## 1. tester drvTest

### Commands

#### Crée le dossier pour le driver

```shell
cd ~/LinEmb/pi4-config/package
mkdir drvTest
cd drvTest
```

#### Crée les fichier de config du driver

```shell
cat << 'EOF' > Config.in
config BR2_PACKAGE_DRVTEST
    bool "drvTest"
    depends on BR2_LINUX_KERNEL
    help
        Hello test help
EOF

cat << 'EOF' > drvTest.desk
name: drvTest
EOF

cat << 'EOF' > drvTest.mk
DRVTEST_VERSION = 1.0
DRVTEST_MODULE_VERSION = 1.0
DRVTEST_SITE = ../projects/gr9/driver
DRVTEST_SITE_METHOD = local
DRVTEST_LICENSE = GPL-2.0
$(eval $(kernel-module))
$(eval $(generic-package))
EOF
```

#### Ajouter le module à la configuration de buildroot

```shell
cd ~/LinEmb/pi4-config/

# le fichier Config.in est déjas crée avec labo5.d
sed -i '$ i\source "$BR2_EXTERNAL_PI4_CONFIG_PATH/package/drvTest/Config.in"' Config.in

```

#### Menu config

```shell
cd ../build-pi4/
make menuconfig
```

Dans le menu `External options -> Custom packages` cocher `drvTest`. Sauvegarder et quitter.

#### Compiler le noyau

```shell
 make -j8
```

# 16.05.2022

### Compiler un fichier source

```shell
 cd project/server
 make
```

### Envoie du fichier compilé sur le RPI

scp test_drvTest rpi@157.26.91.84:/home/rpi/prg

### Charger le module sur le rpi

```shell
ssh rpi@157.26.91.84 # Password : rpi
su # Password : root
modprobe drvTest
```

### Lancer le programme

```shell
cd /home/rpi/prg
su
./test_drvTtest
```
