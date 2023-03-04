# EC2 Instance
resource "aws_instance" "this" {
  count = local.create_instance ? 1 : 0

  # Name and tags
  tags = {
    Name = local.name
  }

  # Application and OS Images (Amazon Machine Image)
  ami           = data.aws_ami.os[count.index].id
  instance_type = var.instance_type
  key_name      = local.key_name

  # Network settings
  subnet_id                   = local.subnet_id
  associate_public_ip_address = true
  vpc_security_group_ids      = local.vpc_security_group_ids

  # Configure storage
  root_block_device {
    volume_size = var.root_volume_size
    volume_type = var.root_volume_type
    iops        = var.root_volume_iops
    throughput  = var.root_volume_throughput
  }

  # Advanced details
  iam_instance_profile = local.iam_instance_profile

  instance_initiated_shutdown_behavior = var.shutdown_behavior

  user_data = try(data.cloudinit_config.this[count.index].rendered, null)

  lifecycle {
    ignore_changes = [
      user_data
    ]
  }
}

# EC2 Key pair
resource "aws_key_pair" "this" {
  count = local.create_key_pair ? 1 : 0

  key_name_prefix = "${local.name}-"
  public_key      = var.public_key
}

# User data
data "cloudinit_config" "this" {
  count = local.create_instance ? (var.enable_volume || var.enable_node || var.enable_logfilegen || var.enable_benchmark ? 1 : 0) : 0

  # Volume
  dynamic "part" {
    for_each = var.enable_volume ? [1] : []
    content {
      filename     = "01-volume.sh"
      content_type = "text/x-shellscript"
      content = templatefile("${path.module}/files/01-volume.sh", {
        mount_point      = var.data_volume_mount_point,
        data_volume_size = var.data_volume_size == 0 ? "null" : var.data_volume_size,
      })
    }
  }

  # Node
  dynamic "part" {
    for_each = var.enable_node ? [1] : []
    content {
      filename     = "02-node.sh"
      content_type = "text/x-shellscript"
      content = templatefile("${path.module}/files/02-node.sh", {
        hostname = local.name
      })
    }
  }

  # Logfilegen
  dynamic "part" {
    for_each = var.enable_logfilegen ? [1] : []
    content {
      filename     = "03-logfilegen.sh"
      content_type = "text/x-shellscript"
      content = templatefile("${path.module}/files/03-logfilegen.sh", {
        logfilegen_version = var.logfilegen_version
      })
    }
  }

  # Benchmark
  dynamic "part" {
    for_each = var.enable_benchmark ? [1] : []
    content {
      filename     = "04-benchmark.sh"
      content_type = "text/x-shellscript"
      content = templatefile("${path.module}/files/04-benchmark.sh", {
        base_dir               = var.data_volume_mount_point,
        instance_type          = var.instance_type,
        data_volume_size       = var.data_volume_size == 0 ? "null" : var.data_volume_size,
        data_volume_type       = var.data_volume_type == "" ? "null" : var.data_volume_type,
        data_volume_iops       = var.data_volume_iops == 0 ? "null" : var.data_volume_iops,
        data_volume_throughput = var.data_volume_throughput == 0 ? "null" : var.data_volume_throughput,
        benchmark_count        = var.benchmark_count,
        benchmark_duration     = var.benchmark_duration,
        benchmark_auto         = var.benchmark_auto,
        results_bucket         = var.results_bucket == null ? "null" : var.results_bucket,
        results_bucket_key     = var.results_bucket_key
      })
    }
  }

  # Docker
  dynamic "part" {
    for_each = var.enable_docker ? [1] : []
    content {
      filename     = "05-docker.sh"
      content_type = "text/x-shellscript"
      content = templatefile("${path.module}/files/05-docker.sh", {
      })
    }
  }
}
