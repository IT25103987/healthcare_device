import React from 'react';
import { LineChart, Line, AreaChart, Area, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer } from 'recharts';

const ChartComponent = ({ title, data, type = 'line', dataKey, stroke = '#0284c7', fill = '#0284c7' }) => {
  if (!data || data.length === 0) {
    return (
      <div className="card">
        <h3 className="text-lg font-semibold mb-4">{title}</h3>
        <p className="text-gray-500 text-center py-8">No data available</p>
      </div>
    );
  }

  return (
    <div className="card">
      <h3 className="text-lg font-semibold mb-4">{title}</h3>
      <ResponsiveContainer width="100%" height={300}>
        {type === 'area' ? (
          <AreaChart data={data}>
            <defs>
              <linearGradient id="colorValue" x1="0" y1="0" x2="0" y2="1">
                <stop offset="5%" stopColor={fill} stopOpacity={0.8}/>
                <stop offset="95%" stopColor={fill} stopOpacity={0}/>
              </linearGradient>
            </defs>
            <CartesianGrid strokeDasharray="3 3" />
            <XAxis dataKey="time" />
            <YAxis />
            <Tooltip />
            <Area type="monotone" dataKey={dataKey} stroke={stroke} fillOpacity={1} fill="url(#colorValue)" />
          </AreaChart>
        ) : (
          <LineChart data={data}>
            <CartesianGrid strokeDasharray="3 3" />
            <XAxis dataKey="time" />
            <YAxis />
            <Tooltip />
            <Legend />
            <Line type="monotone" dataKey={dataKey} stroke={stroke} dot={{ r: 4 }} activeDot={{ r: 6 }} />
          </LineChart>
        )}
      </ResponsiveContainer>
    </div>
  );
};

export default ChartComponent;
