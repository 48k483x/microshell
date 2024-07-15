#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int	err(char *msg)
{
	while (*msg)
		write(2, msg++, 1);
	return (1);
}

int	cd(char **args, int i)
{
	if (i != 2)
		return (err("error: cd: bad arguments\n"));
	if (chdir(args[1]) == -1)
		return (err("error: cd: cannot change directory to "), err(args[1]), err("\n"));
	return (0);
}

int	exec(char **args, char **env, int i)
{
	pid_t	pid;
	int		fd[2];
	int		status;

	int has_pipe = args[i] && !strcmp(args[i], "|");
	if (!has_pipe && !strcmp(*args, "cd"))
		return (cd(args, i));
	if (has_pipe && pipe(fd) == -1)
		return (err("error: fatal\n"));
	pid = fork();
	if (!pid)
	{
		args[i] = 0;
		if (has_pipe && (dup2(fd[1], 1) || close(fd[0]) == -1 || close(fd[1] == -1)))
			return (err("error: fatal\n"));
		if (!strcmp(*args, "cd"))
			return (cd(args, i));
		execve(*args, args, env);
		return (err("error: cannot execute "), err(*args), err("\n"));
	}
	waitpid(pid, &status, 0);
	if (has_pipe && (dup2(fd[0], 0) || close(fd[0]) == -1 || close(fd[1] == -1)))
		return (err("error: fatal\n"));
	return WIFEXITED(status) && WEXITSTATUS(status);
}

int	main(int ac, char **av, char **envp)
{
	int	i = 0;
	int status = 0;

	if (ac > 1)
	{
		while (av[i] && av[++i])
		{
			av += i;
			i = 0;
			while (av[i] && strcmp(av[i], "|") && strcmp(av[i], ";"))
				i++;
			if (i)
				status = exec(av, envp,i);
		}
	}
	return (status);
}