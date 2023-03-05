# Logfilegen
module "instance-01" {
  source = "./modules/vm"

  create                  = var.create
  region                  = var.region
  name                    = var.name
  az                      = var.az
  subnet_id               = var.subnet_id
  vpc_security_group_ids  = var.vpc_security_group_ids
  allow_ssh               = var.allow_ssh
  allow_tcp_ports         = var.allow_tcp_ports
  os_name                 = var.os_name
  instance_architecture   = var.instance_architecture
  instance_type           = var.instance_type
  key_name                = var.key_name
  public_key              = var.public_key
  root_volume_size        = var.root_volume_size
  root_volume_type        = var.root_volume_type
  root_volume_iops        = var.root_volume_iops
  root_volume_throughput  = var.root_volume_throughput
  shutdown_behavior       = var.shutdown_behavior
  enable_volume           = var.enable_volume
  enable_node             = var.enable_node
  enable_logfilegen       = var.enable_logfilegen
  enable_benchmark        = var.enable_benchmark
  enable_docker           = var.enable_docker
  logfilegen_version      = var.logfilegen_version
  benchmark_count         = var.benchmark_count
  benchmark_duration      = var.benchmark_duration
  benchmark_auto          = var.benchmark_auto
  results_bucket          = var.results_bucket
  results_bucket_key      = var.results_bucket_key
  data_volume_size        = var.data_volume_size
  data_volume_type        = var.data_volume_type
  data_volume_iops        = var.data_volume_iops
  data_volume_throughput  = var.data_volume_throughput
  data_volume_mount_point = var.data_volume_mount_point
  data_volume_device_name = var.data_volume_device_name
}


# module "instance-02" {
#   source = "./modules/vm"

#   name                  = "logfilegen-2"
#   os_name               = "amazon"
#   instance_architecture = "arm64"
#   instance_type         = "t4g.micro"
#   key_name              = var.key_name
#   allow_tcp_ports       = [8080]
#   logfilegen_version    = "2.0.0"
# }
