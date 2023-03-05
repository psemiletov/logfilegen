# Main
variable "create" {
  description = "Whether to create resources."
  type        = bool
  default     = true
}

variable "region" {
  description = "Region to be used for created resources."
  type        = string
  default     = "eu-west-1"
}

variable "name" {
  description = "Name to be used for created resources."
  type        = string
  default     = "logfilegen"
}

# VPC
variable "az" {
  description = "AZ to start the instance in."
  type        = string
  default     = ""
}

variable "subnet_id" {
  description = "Subnet ID where tu run the instance"
  type        = string
  default     = null
}

variable "vpc_security_group_ids" {
  description = "Security Groups IDS to attach to the instance"
  type        = list(string)
  default     = []
}

variable "allow_ssh" {
  description = "Whether to allow SSH access to the instance."
  type        = bool
  default     = true
}

variable "allow_tcp_ports" {
  description = "List of TCP ports to be added to the security group."
  type        = list(number)
  default     = []
}

# EC2 Instance
variable "os_name" {
  description = "OS name."
  type        = string
  default     = "ubuntu"
}

variable "instance_architecture" {
  description = "EC2 instance OS architecture."
  type        = string
  default     = "x86_64"
}

variable "instance_type" {
  description = "The type of the instance."
  type        = string
  default     = "t3.micro"
}

variable "key_name" {
  description = "AWS EC2 Key Pair name to use for the instance."
  type        = string
  default     = null
}

variable "public_key" {
  description = "The public key material."
  type        = string
  default     = null
}

variable "root_volume_size" {
  description = "Size of the volume in gibibytes (GiB)."
  type        = number
  default     = null
}

variable "root_volume_type" {
  description = "Type of volume. Valid values include standard, gp2, gp3, io1, io2, sc1, or st1. Defaults to gp2."
  type        = string
  default     = "gp3"
}

variable "root_volume_iops" {
  description = "Amount of provisioned IOPS. Only valid for volume_type of io1, io2 or gp3."
  type        = number
  default     = null
}

variable "root_volume_throughput" {
  description = "Throughput to provision for a volume in mebibytes per second (MiB/s). This is only valid for volume_type of gp3."
  type        = number
  default     = null
}

variable "shutdown_behavior" {
  description = "Shutdown behavior for the instance."
  type        = string
  default     = "terminate"
}

# User data
variable "enable_volume" {
  description = "Whether to run volume script."
  type        = bool
  default     = true
}

variable "enable_node" {
  description = "Whether to run node script."
  type        = bool
  default     = true
}

variable "enable_logfilegen" {
  description = "Whether to run logfilegen script."
  type        = bool
  default     = true
}

variable "enable_benchmark" {
  description = "Whether to run benchmark script."
  type        = bool
  default     = false
}

variable "enable_docker" {
  description = "Whether to install Docker."
  type        = bool
  default     = false
}

variable "logfilegen_version" {
  description = "Logfilegen version to install from GitHub sources."
  type        = string
  default     = "main"
}

variable "benchmark_count" {
  description = "Number of tests to be performed."
  type        = number
  default     = 5
}

variable "benchmark_duration" {
  description = "Duration of the tests."
  type        = number
  default     = 60
}

variable "benchmark_auto" {
  description = "Shutdown the instance after test is finished."
  type        = bool
  default     = false
}

variable "results_bucket" {
  description = "S3 bucket for benchmark results."
  type        = string
  default     = null
}

variable "results_bucket_key" {
  description = "S3 bucket key for benchmark results."
  type        = string
  default     = "benchmark"
}

# EBS Volume
variable "data_volume_size" {
  description = "Size of the volume in gibibytes (GiB)."
  type        = number
  default     = 0
}

variable "data_volume_type" {
  description = "Type of volume. Valid values include standard, gp2, gp3, io1, io2, sc1, or st1. Defaults to gp2."
  type        = string
  default     = ""
}

variable "data_volume_iops" {
  description = "Amount of provisioned IOPS. Only valid for volume_type of io1, io2 or gp3."
  type        = number
  default     = 0
}

variable "data_volume_throughput" {
  description = "Throughput to provision for a volume in mebibytes per second (MiB/s). This is only valid for volume_type of gp3."
  type        = number
  default     = 0
}

variable "data_volume_mount_point" {
  description = "Mount point of the data volume."
  type        = string
  default     = "/data"
}

# EBS Volume Attachment
variable "data_volume_device_name" {
  description = "Name of the device to mount."
  type        = string
  default     = "/dev/sdf"
}
