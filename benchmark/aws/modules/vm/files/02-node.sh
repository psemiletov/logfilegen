#! /bin/bash

# Variables
hostname="${hostname}"


# Set hostname
hostnamectl set-hostname "$hostname"


# Identify OS
distr_id=$(hostnamectl | awk -F ': ' '/Operating System/ {print $2}')
if [[ "$${distr_id,,}" == *"ubuntu"* ]]; then
  os=ubuntu
elif [[ "$${distr_id,,}" == *"amazon"* ]]; then
  os=amazon
else
  os=unknown
fi


# Install packages
case "$os" in
  ubuntu) apt update
          apt install -y git make g++ cmake pkg-config systemd-coredump unzip jq
          ;;

  amazon) yum -y git make gcc-c++ systemd-coredump unzip jq
          ;;

       *) echo "Unknown OS"
          exit 1
          ;;
esac


# Install AWS CLI
arch=$(hostnamectl | awk -F ': ' '/Architecture/ {print $2}')
case "$arch" in
  x86-64) curl "https://awscli.amazonaws.com/awscli-exe-linux-x86_64.zip" -o "awscliv2.zip"
          unzip awscliv2.zip
          sudo ./aws/install
          ;;

  arm64) curl https://awscli.amazonaws.com/awscli-exe-linux-aarch64.zip -o awscliv2.zip
         unzip awscliv2.zip
         sudo ./aws/install
         ln -s /usr/local/bin/aws /usr/bin/aws
          ;;

       *) echo "Unknown architecture"
          exit 1
          ;;
esac
