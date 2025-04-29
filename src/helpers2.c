/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers2.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adeters <adeters@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/28 18:30:51 by adeters           #+#    #+#             */
/*   Updated: 2025/04/29 14:10:08 by adeters          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

int	ft_isdigit(int c)
{
	if (c >= '0' && c <= '9')
		return (2048);
	return (0);
}

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t	i;

	if (!s1 || !s2)
		return (0);
	i = 0;
	while (s1[i] && i < n)
	{
		if (s1[i] != s2[i])
			return ((unsigned char)s1[i] - (unsigned char)s2[i]);
		i++;
	}
	while (s2[i] && i < n)
	{
		if (s1[i] != s2[i])
			return ((unsigned char)s1[i] - (unsigned char)s2[i]);
		i++;
	}
	return (0);
}

int	get_next_delay(t_data *data)
{
	int	res;

	pthread_mutex_lock(&data->mutexes[data->nbp + 5]);
	res = data->next_delay;
	return (pthread_mutex_unlock(&data->mutexes[data->nbp + 5]), res);
}

void	do_on_sleep_fail(t_philo **ps)
{
	pthread_mutex_lock(&(*ps)->mutexes[(*ps)->data->nbp + 1]);
	(*ps)->data->is_kil = true;
	pthread_mutex_unlock(&(*ps)->mutexes[(*ps)->data->nbp + 1]);
	p_str_fd(2, "Error: usleep function failed!\n");
}
