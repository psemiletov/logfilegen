# IAM Role
resource "aws_iam_role" "this" {
  count = local.create_instance_profile ? 1 : 0

  name = local.iam_resources_name
  path = "/"

  assume_role_policy = jsonencode({
    Version = "2012-10-17"
    Statement = [
      {
        Action = "sts:AssumeRole"
        Effect = "Allow"
        Sid    = ""
        Principal = {
          Service = "ec2.amazonaws.com"
        }
      },
    ]
  })
}

# IAM Instance Profile
resource "aws_iam_instance_profile" "this" {
  count = local.create_instance_profile ? 1 : 0

  name = local.iam_resources_name
  role = aws_iam_role.this[count.index].name
}

# IAM Policy
resource "aws_iam_role_policy" "this" {
  count = local.create_instance_profile ? 1 : 0

  name = local.iam_resources_name
  role = aws_iam_role.this[count.index].id

  policy = jsonencode({
    Version = "2012-10-17"
    Statement = [
      {
        Sid      = "ListObjectsInBucket",
        Action   = ["s3:ListBucket"],
        Effect   = "Allow",
        Resource = ["arn:aws:s3:::${var.results_bucket}"]
      },
      {
        Sid      = "AllObjectActions",
        Action   = "s3:*Object",
        Effect   = "Allow"
        Resource = ["arn:aws:s3:::${var.results_bucket}/*"]
      }
    ]
  })
}
