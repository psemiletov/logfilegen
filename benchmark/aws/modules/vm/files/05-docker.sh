#! /bin/bash

# Identify OS
distrib_id=$(hostnamectl | awk -F ': ' '/Operating System/ {print $2}')
if [[ "$${distrib_id,,}" == *"ubuntu"* ]]; then
  os=ubuntu
elif [[ "$${distrib_id,,}" == *"amazon"* ]]; then
  os=amazon
else
  os=unknown
fi


# Install Docker
case "$os" in
  ubuntu) apt update
          apt install -y ca-certificates curl gnupg lsb-release clang
          mkdir -m 0755 -p /etc/apt/keyrings
          curl -fsSL https://download.docker.com/linux/ubuntu/gpg | gpg --dearmor -o /etc/apt/keyrings/docker.gpg
          echo \
            "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.gpg] https://download.docker.com/linux/ubuntu \
            $(lsb_release -cs) stable" | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null
          apt-get update
          apt-get install -y docker-ce docker-ce-cli containerd.io docker-buildx-plugin docker-compose-plugin
          systemctl enable docker
          systemctl start docker
          ;;

  amazon) amazon-linux-extras install docker
          systemctl enable docker
          systemctl start docker
          ;;

       *) echo "Unknown OS"
          exit 1
          ;;
esac


# Install Docker compose
curl -L "https://github.com/docker/compose/releases/latest/download/docker-compose-$(uname -s)-$(uname -m)" -o docker-compose
install -m 0755 docker-compose /usr/bin/docker-compose
