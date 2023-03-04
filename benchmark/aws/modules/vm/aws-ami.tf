# Locals
locals {
  architecture_map = {
    "x86_64" = var.os_name == "amazon" ? "x86_64" : "amd64"
    "arm64"  = "arm64"
  }

  os_name_map = {
    ubuntu-16 = "ubuntu/images/hvm-ssd/ubuntu-*-16.04-${local.architecture}-server-*"
    ubuntu-18 = "ubuntu/images/hvm-ssd/ubuntu-*-18.04-${local.architecture}-server-*"
    ubuntu-20 = "ubuntu/images/hvm-ssd/ubuntu-*-20.04-${local.architecture}-server-*"
    ubuntu-22 = "ubuntu/images/hvm-ssd/ubuntu-*-22.04-${local.architecture}-server-*"
    ubuntu    = "ubuntu/images/hvm-ssd/ubuntu-*-22.04-${local.architecture}-server-*"
    amazon    = "amzn2-*-${local.architecture}-*"
  }

  os_owner_map = {
    ubuntu-16 = "099720109477"
    ubuntu-18 = "099720109477"
    ubuntu-20 = "099720109477"
    ubuntu-22 = "099720109477"
    ubuntu    = "099720109477"
    amazon    = "137112412989"
  }

  architecture = lookup(local.architecture_map, var.instance_architecture)
  os_owner     = lookup(local.os_owner_map, var.os_name)
  os_name      = lookup(local.os_name_map, var.os_name)

  user_map = {
    ubuntu-16 = "ubuntu"
    ubuntu-18 = "ubuntu"
    ubuntu-20 = "ubuntu"
    ubuntu-22 = "ubuntu"
    ubuntu    = "ubuntu"
    amazon    = "ec2-user"
  }

  username = lookup(local.user_map, var.os_name)
}

# AMI
data "aws_ami" "os" {
  count = local.create_instance ? 1 : 0

  most_recent = true
  owners      = [local.os_owner]

  filter {
    name   = "virtualization-type"
    values = ["hvm"]
  }

  filter {
    name   = "name"
    values = [local.os_name]
  }
}
