# VPC - Get Default
data "aws_vpc" "default" {
  count = local.create ? 1 : 0

  default = true
}

# Subnet in AZ
data "aws_subnets" "this" {
  count = local.create && var.az != "" ? 1 : 0

  filter {
    name   = "vpc-id"
    values = [data.aws_vpc.default[count.index].id]
  }

  filter {
    name   = "availabilityZone"
    values = ["${local.region}${var.az}"]
  }
}

# Security group - Get default
data "aws_security_group" "default" {
  count = local.create ? 1 : 0

  vpc_id = data.aws_vpc.default[count.index].id
  name   = "default"
}

# Security group
resource "aws_security_group" "this" {
  count = local.create_security_group ? 1 : 0

  name        = local.name
  description = "${local.name} access"
  vpc_id      = data.aws_vpc.default[count.index].id

  ingress {
    protocol  = -1
    self      = true
    from_port = 0
    to_port   = 0
  }

  dynamic "ingress" {
    for_each = var.allow_ssh ? [1] : []
    content {
      description = "SSH - Any"
      from_port   = 22
      to_port     = 22
      protocol    = "tcp"
      cidr_blocks = ["0.0.0.0/0"]
    }
  }

  dynamic "ingress" {
    for_each = var.allow_tcp_ports
    content {
      description = "TCP ${ingress.value} - Any"
      from_port   = ingress.value
      to_port     = ingress.value
      protocol    = "tcp"
      cidr_blocks = ["0.0.0.0/0"]
    }
  }

  ingress {
    description     = "Default SG - Any"
    from_port       = 0
    to_port         = 0
    protocol        = -1
    security_groups = [data.aws_security_group.default[count.index].id]
  }

  egress {
    from_port   = 0
    to_port     = 0
    protocol    = "-1"
    cidr_blocks = ["0.0.0.0/0"]
  }

  tags = {
    Name = local.name
  }

  lifecycle {
    create_before_destroy = true
  }
}
