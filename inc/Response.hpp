/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mschaub <mschaub@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/10 13:26:24 by mschaub           #+#    #+#             */
/*   Updated: 2023/11/10 16:13:41 by mschaub          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

class Response {
    public:
        Response();
        ~Response();
    private:
        std::string _version;
        std::string _code;
        std::string _status;
};