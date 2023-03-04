# Locals
locals {
  create                  = var.create
  create_security_group   = local.create && length(var.vpc_security_group_ids) == 0 && (var.allow_ssh || length(var.allow_tcp_ports) > 0)
  create_instance         = local.create
  create_instance_profile = local.create_instance && var.results_bucket != null ? true : false
  create_key_pair         = local.create_instance && var.public_key != null ? true : false
  create_data_volume      = local.create_instance && var.data_volume_size > 0 ? true : false
  name                    = lower(replace(var.name, " ", "-"))
  region                  = var.region
  subnet_id               = var.subnet_id == null ? var.az == "" ? null : element(data.aws_subnets.this[0].ids, 0) : var.subnet_id
  vpc_security_group_ids  = local.create_security_group ? [aws_security_group.this[0].id] : length(var.vpc_security_group_ids) > 0 ? var.vpc_security_group_ids : try([data.aws_security_group.default[0].id], [])
  key_name                = local.create_key_pair ? aws_key_pair.this[0].key_name : var.key_name
  iam_instance_profile    = local.create_instance_profile ? aws_iam_instance_profile.this[0].name : null
  iam_resources_name      = "${local.name}-${var.instance_type}-${local.region}"
}
