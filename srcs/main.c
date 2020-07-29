/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: justasze <justasze@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/10 13:39:50 by justasze          #+#    #+#             */
/*   Updated: 2019/01/24 11:56:58 by justasze         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "ft_traceroute.h"


static void		parse_option_bis(char **av, size_t *arg, size_t i,
		t_traceroute_context *context)
{
	if (av[*arg][i] == 'q')
	{
		*arg += 1;
		context->nb_queries = (uint32_t)ft_atoi(av[*arg]);
		if (context->nb_queries == 0)
		{
			dprintf(2, CANT_HANDLE, 'q', av[*arg], *arg);
			exit(EXIT_FAILURE);
		}
		else if (context->nb_queries > 10)
		{
			dprintf(2, MAX_QUERIES);
			exit(EXIT_FAILURE);
		}
	}
	else if (av[*arg][i] == 'm')
	{
		*arg += 1;
		context->nb_hops_max = (uint32_t)ft_atoi(av[*arg]);
		if (context->nb_hops_max == 0)
		{
			dprintf(2, CANT_HANDLE, 'm', av[*arg], *arg);
			exit(EXIT_FAILURE);
		}
		else if (context->nb_hops_max > 255)
		{
			dprintf(2, MAX_HOPS);
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		dprintf(2, BAD_OPTION, av[*arg][i], *arg);
		exit(EXIT_FAILURE);
	}
}

static void		parse_option(char **av, size_t *arg,
		t_traceroute_context *context)
{
	size_t	i;

	(void)context;
	i = 1;
	while (av[*arg][i])
	{
		if (av[*arg][i] == 'h')
		{
			dprintf(2, USAGE, av[0]);
			exit(EXIT_SUCCESS);
		}
		else if (av[*arg][i] == 'n')
			context->flags |= NO_REV_DNS_FLAG;
		else if (av[*arg][i] == 'f')
		{
			*arg += 1;
			context->first_hop = (uint32_t)ft_atoi(av[*arg]);
			if (context->first_hop == 0)
			{
				dprintf(2, CANT_HANDLE, 'f', av[*arg], *arg);
				exit(EXIT_FAILURE);
			}
			return ;
		}
		else
		{
			parse_option_bis(av, arg, i, context);
			return ;
		}
		i++;
	}
}

static size_t	get_arguments(int ac, char **av, t_traceroute_context *context)
{
	size_t	i;

	i = 1;
	while (i < (size_t)ac)
	{
		if (av[i][0] != '-')
			return (i);
		parse_option(av, &i, context);
		i++;
	}
	return (i);
}

static void		init_context(t_traceroute_context *context)
{
	context->flags = 0;
	context->id = getpid();
	context->error = 0;
	context->first_hop = 1;
	context->nb_hops_max = 30;
	context->nb_queries = 3;
	context->packet_size = 60;
	context->host_infos = NULL;
}

int				main(int ac, char **av)
{
	struct addrinfo			hints;
	size_t					host_arg;
	t_traceroute_context	context;

	if (ac < 2)
	{
		dprintf(2, USAGE, av[0]);
		return (EXIT_FAILURE);
	}
	else if (getuid() != 0)
	{
		dprintf(2, ROOT_NEEDED);
		return (EXIT_FAILURE);
	}
	init_context(&context);
	host_arg = get_arguments(ac, av, &context);
	context.host_name = av[host_arg];
	ft_bzero(&hints, sizeof(struct addrinfo));
	hints.ai_flags = AI_CANONNAME;
	hints.ai_family = AF_INET;
	if (getaddrinfo(av[host_arg], NULL, &hints, &context.host_infos) == -1)
		dprintf(2, GETADDRINFO_ERROR);
	else if (context.host_infos != NULL)
		return (traceroute(&context));
	else
		dprintf(2, UNKNOWN_HOST_FORMAT, context.host_name, context.host_name,
				host_arg, host_arg);
	return (EXIT_FAILURE);
}
