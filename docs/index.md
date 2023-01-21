# logfilegen

The server log file generator

logfilegen is controlled by the configuration variables and templates. The usual way to run logfilegen from the command line is, for example:


```console
logfilegen --template=mytemplate.tp --duration=20 --rate=100 --logfile=out.log
```

The **configuration variables** affects how logfilegen works, and **templates** affects the  generation of log file.

There are many customization ways for logfilegen. To master them, you can read the following chapters:

[Using configuration](config.md)

[Using templates](templates.md)
