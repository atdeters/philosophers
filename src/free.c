/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: andreas <andreas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 12:26:11 by andreas           #+#    #+#             */
/*   Updated: 2025/04/26 14:42:23 by andreas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

void	free_philos(t_philo **lst, int nbp)
{
	int	i;

	i = 0;
	while (lst && lst[i] && i < nbp)
	{
		free(lst[i]);
		i++;
	}
	free(lst);
	lst = NULL;
}
