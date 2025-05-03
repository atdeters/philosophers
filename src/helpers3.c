/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers3.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adeters <adeters@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 17:26:06 by adeters           #+#    #+#             */
/*   Updated: 2025/05/03 17:38:49 by adeters          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

int	ft_usleep(int time, t_data *data)
{
	int	i;

	i = 0;
	while (i < time / 1000)
	{
		pthread_mutex_lock(&data->mutexes[data->nbp + 1]);
		if (data->is_kil)
		{
			pthread_mutex_unlock(&data->mutexes[data->nbp + 1]);
			return (1);
		}
		pthread_mutex_unlock(&data->mutexes[data->nbp + 1]);
		usleep(1000);
		i++;
	}
	return (0);
}
