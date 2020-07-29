/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_traceroute.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: justasze <justasze@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/10 13:38:15 by justasze          #+#    #+#             */
/*   Updated: 2019/01/25 15:24:47 by justasze         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_TRACEROUTE_H

# define FT_TRACEROUTE_H

# include "libft.h"
# include <arpa/inet.h>
# include <netdb.h>
# include <netinet/in.h>
# include <netinet/ip_icmp.h>
# include <sys/param.h>
# include <sys/socket.h>
# include <sys/stat.h>
# include <sys/time.h>
# include <sys/types.h>

# define USAGE "Usage:\n %s [ -h ] host [ packetlen ]\n"

# define ROOT_NEEDED "You need to be root to launch this program\n"
# define GETADDRINFO_ERROR "Error with getaddrinfo\n"

# define MAX_QUERIES "no more than 10 probes per hop\n"
# define MAX_HOPS "max hops cannot be more than 255\n"
# define FIRST_HOP_OOR "first hop out of range\n"
# define CANT_HANDLE "Cannot handle `-%c' option with arg `%s' (argc %zu)\n"
# define BAD_OPTION "Bad option `-%c` (argc %zu)\n"

# define UNKNOWN_HOST1 "%s: Name or service not known.\n"
# define UNKNOWN_HOST2 "Cannot handle \"host\" cmdline arg `%s'"
# define UNKNOWN_HOST3 " on position %zu (argc %zu)\n"
# define UNKNOWN_HOST_FORMAT UNKNOWN_HOST1 UNKNOWN_HOST2 UNKNOWN_HOST3

# define LAUNCH_FORMAT "traceroute to %s (%s), %zu hops max, %zu byte packets\n"
# define ADDR_FORMAT " %s (%s)"
# define NO_REV_DNS_FORMAT " %s"
# define RTT_FORMAT "  %.3fms"

# define CANT_CREATE_SOCKET "Error: couldn't create socket.\n"
# define CANT_SET_TIMEOUT "Error: couldn't set receive timeout.\n"
# define CANT_SET_TTL "Error: couldn't set time to live.\n"

# define SENDING_ERROR "Error while sending packet\n"

# define NO_REV_DNS_FLAG 0x1

typedef struct		s_traceroute_context
{
	int				socket;
	int				flags;
	int				error;
	pid_t			id;
	size_t			first_hop;
	size_t			nb_hops_max;
	size_t			nb_queries;
	size_t			packet_size;
	struct addrinfo	*host_infos;
	char			*host_name;
	struct timeval	time_at_sending;
	struct timeval	time_at_reception;
	char			host_addr[INET_ADDRSTRLEN];
	char			reverse_dns[NI_MAXHOST];
}					t_traceroute_context;

int					ping(t_traceroute_context *context);
int					traceroute(t_traceroute_context *context);

#endif
