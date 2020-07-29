/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   traceroute.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: justasze <justasze@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/10 14:18:11 by justasze          #+#    #+#             */
/*   Updated: 2019/01/25 15:19:14 by justasze         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "ft_traceroute.h"

static int			fatal(t_traceroute_context *context, char *msg)
{
	dprintf(2, "%s", msg);
	freeaddrinfo(context->host_infos);
	close(context->socket);
	exit(EXIT_FAILURE);
}

static void			get_host_ip_str(t_traceroute_context *context, char *buffer)
{
	void				*addr;
	struct sockaddr_in	*ipv4;
	struct sockaddr_in6	*ipv6;

	if (context->host_infos->ai_family == AF_INET)
	{
		addr = context->host_infos->ai_addr;
		ipv4 = (struct sockaddr_in *)addr;
		addr = &(ipv4->sin_addr);
	}
	else
	{
		addr = context->host_infos->ai_addr;
		ipv6 = (struct sockaddr_in6 *)addr;
		addr = &(ipv6->sin6_addr);
	}
	inet_ntop(context->host_infos->ai_family, addr, buffer,
			context->host_infos->ai_addrlen);
}

static void			create_socket(t_traceroute_context *context)
{
	struct timeval	timeout;

	context->socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (context->socket == -1)
		fatal(context, CANT_CREATE_SOCKET);
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	if (setsockopt(context->socket, SOL_SOCKET, SO_RCVTIMEO, &timeout,
			sizeof(struct timeval)) != 0)
		fatal(context, CANT_SET_TIMEOUT);
}
int			traceroute(t_traceroute_context *context)
{
	socklen_t	time_to_live;

	time_to_live = context->first_hop;
	create_socket(context);
	get_host_ip_str(context, context->host_addr);
	printf(LAUNCH_FORMAT, context->host_name, context->host_addr,
			context->nb_hops_max, context->packet_size);
	if (context->first_hop > context->nb_hops_max)
		fatal(context, FIRST_HOP_OOR);
	while (time_to_live <= context->nb_hops_max)
	{
		if (setsockopt(context->socket, SOL_IP, IP_TTL, &time_to_live,
				sizeof(time_to_live)) != 0)
			fatal(context, CANT_SET_TTL);
		printf("%2u ", time_to_live);
		if (ping(context) == 1 || context->error == 1)
			break ;
		time_to_live++;
	}
	freeaddrinfo(context->host_infos);
	close(context->socket);
	return (0);
}

