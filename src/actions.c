/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: andreas <andreas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 01:34:34 by andreas           #+#    #+#             */
/*   Updated: 2025/04/29 01:34:37 by andreas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

int	act_think_and_sleep(t_philo *p)
{
	if (!p_log(p->data, p->philo_nb, SLEEP, p->mutexes))
		return (0);
	usleep(p->data->tts);
	if (!p_log(p->data, p->philo_nb, THINK, p->mutexes))
		return (0);
	return (1);
}
