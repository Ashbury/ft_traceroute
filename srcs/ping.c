/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: justasze <justasze@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/11 15:51:00 by justasze          #+#    #+#             */
/*   Updated: 2019/01/25 15:24:07 by justasze         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_traceroute.h"

static uint16_t	checksum(uint16_t *buffer, size_t len)
{
	uint32_t	sum;

	sum = 0;
	while (len > 1)
	{
		sum += *buffer;
		buffer++;
		len -= 2;
	}
	if (len == 1)
		sum += *(unsigned char*)buffer;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	return ((uint16_t)~sum);
}

static int		sending_error(t_traceroute_context *context)
{
	dprintf(2, SENDING_ERROR);
	close(context->socket);
	freeaddrinfo(context->host_infos);
	exit(EXIT_FAILURE);
}

static void		reverse_dns_lookup(t_traceroute_context *context,
		uint32_t ip_addr)
{
	struct sockaddr_in	temp_addr;
	socklen_t			len;

	temp_addr.sin_family = AF_INET;
	temp_addr.sin_addr.s_addr = ip_addr;
	len = sizeof(struct sockaddr_in);
	if (getnameinfo((struct sockaddr *)&temp_addr, len, context->reverse_dns,
			NI_MAXHOST, NULL, 0, NI_NOFQDN) != 0)
		ft_strcpy(context->reverse_dns, context->host_addr);
}

static void		print_icmp_error(t_traceroute_context *context, uint8_t code)
{
	context->error = 1;
	printf(" !");
	switch(code)
	{
		case ICMP_NET_UNREACH:
			printf("N");
			break;
		case ICMP_HOST_UNREACH:
			printf("H");
			break ;
		case ICMP_PROT_UNREACH:
			printf("PR");
			break ;
		case ICMP_PORT_UNREACH:
			printf("PO");
			break ;
		default:
			printf("?");
	}
}

static float	compute_road_trip_time(t_traceroute_context *context)
{
	return ((context->time_at_reception.tv_usec
					- context->time_at_sending.tv_usec) / 1000.0f
					+ (context->time_at_reception.tv_sec
					- context->time_at_sending.tv_sec) * 1000.0f);
}

static int		check_response(t_traceroute_context *context,
		struct iphdr *ip_header, struct icmphdr *icmp_header)
{
	char			*buffer;	
	struct icmphdr	*origin_header;
	char			host_addr[INET_ADDRSTRLEN];

	buffer = (char*)icmp_header;
	origin_header = (struct icmphdr *)(buffer + sizeof(struct icmphdr)
			+ sizeof(struct iphdr));
	if (origin_header->un.echo.id == context->id)
	{
		inet_ntop(AF_INET, &ip_header->saddr, host_addr,
				INET_ADDRSTRLEN);
		if (ft_strcmp(host_addr, context->host_addr) != 0)
		{
			inet_ntop(AF_INET, &ip_header->saddr, context->host_addr,
				INET_ADDRSTRLEN);
			reverse_dns_lookup(context, ip_header->saddr);
			if (context->flags & NO_REV_DNS_FLAG)
				printf(NO_REV_DNS_FORMAT, context->host_addr);
			else
				printf(ADDR_FORMAT, context->reverse_dns, context->host_addr);
		}
		printf(RTT_FORMAT, compute_road_trip_time(context));
		if (icmp_header->type == ICMP_DEST_UNREACH)
			print_icmp_error(context, icmp_header->code);
		return (0);
	}
	return (-1);
}

static int		get_response(t_traceroute_context *context)
{
	char			buffer[128];
	ssize_t			ret;
	struct iphdr	*ip_header;
	struct icmphdr	*icmp_header;
	char			host_addr[INET_ADDRSTRLEN];

	ret = recvfrom(context->socket, buffer, 128, 0,
			NULL, NULL);
	if (ret > 0)
	{
		gettimeofday(&context->time_at_reception, NULL);
		ip_header = (struct iphdr *)buffer;
		icmp_header = (struct icmphdr *)(buffer + sizeof(struct iphdr));
		if (icmp_header->un.echo.id == context->id)
		{
			inet_ntop(AF_INET, &ip_header->saddr, host_addr,
					INET_ADDRSTRLEN);
			if (ft_strcmp(host_addr, context->host_addr) != 0)
			{
				inet_ntop(AF_INET, &ip_header->saddr, context->host_addr,
					INET_ADDRSTRLEN);
				reverse_dns_lookup(context, ip_header->saddr);
				if (context->flags & NO_REV_DNS_FLAG)
					printf(NO_REV_DNS_FORMAT, context->host_addr);
				else
					printf(ADDR_FORMAT, context->reverse_dns, context->host_addr);
			}
			printf(RTT_FORMAT, compute_road_trip_time(context));
			return (1);
		}
		else
		{
			ret = check_response(context, ip_header, icmp_header);
			if (ret == -1)
				return (get_response(context));
			return (ret);
		}
	}
	else
	{
		printf(" *");
		return (0);
	}
}

int				ping(t_traceroute_context *context)
{
	char			*packet;
	struct icmphdr	*icmp_header;
	ssize_t			ret;
	size_t			i;

	i = 0;
	while (i < context->nb_queries)
	{
		packet = ft_memalloc(context->packet_size);
		icmp_header = (struct icmphdr *)packet;
		icmp_header->type = ICMP_ECHO;
		icmp_header->un.echo.id = context->id;
		icmp_header->un.echo.sequence = htons(1);
		icmp_header->checksum = checksum((uint16_t*)packet, context->packet_size);
		gettimeofday(&context->time_at_sending, NULL);
		ret = sendto(context->socket, packet, context->packet_size, 0,
				context->host_infos->ai_addr, sizeof(struct sockaddr_in));
		free(packet);
		if (ret < 1)
			sending_error(context);
		ret = get_response(context);
		i++;
	}
	printf("\n");
	return (ret);
}
