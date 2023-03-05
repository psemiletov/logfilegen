# EBS Volume
resource "aws_ebs_volume" "this" {
  count = local.create_data_volume ? 1 : 0

  type              = var.data_volume_type == "" ? null : var.data_volume_type
  size              = var.data_volume_size == 0 ? null : var.data_volume_size
  iops              = var.data_volume_iops == 0 ? null : var.data_volume_iops
  throughput        = var.data_volume_throughput == 0 ? null : var.data_volume_throughput
  availability_zone = aws_instance.this[count.index].availability_zone

  tags = {
    Name = "${local.name} - ${var.data_volume_mount_point}"
  }
}

# EBS Volume Attachment
resource "aws_volume_attachment" "this" {
  count = local.create_data_volume ? 1 : 0

  instance_id = aws_instance.this[count.index].id
  device_name = var.data_volume_device_name
  volume_id   = aws_ebs_volume.this[count.index].id
}
