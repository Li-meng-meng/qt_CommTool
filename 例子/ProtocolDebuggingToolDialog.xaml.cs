using CommNamespace;
using DataManager.Model;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Runtime.InteropServices.ComTypes;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Markup;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Threading;
using UAV_UP_Sort.Helpers;
using YourApp;
using static CommNamespace.Comm.v0_comm_callbak_t;
using static UAV_UP_Sort.UserCommlmpl;

namespace UAV_UP_Sort.windows
{
    public partial class ProtocolDebuggingToolDialog : Window
    {
        private ObservableCollection<ProtocolCommand> _allCommands;
        private ObservableCollection<ProtocolCommand> _filteredCommands;
        public static ProtocolDebuggingToolDialog Instance { get; private set; }
        private List<RadioButton> _idRadioButtons;
        private List<string> _receivedDataHistory;
        private ProtocolCommand _currentCmd;
        private const int MaxHistoryCount = 20;

        private byte Cmd = 0;
        private byte Id = 0;
        private string CurrentData;
        private byte[] CurrentDataBytes;
        private DateTime _lastDataReceiveTime = DateTime.Now;
        private readonly TimeSpan _dataTimeout = TimeSpan.FromMilliseconds(1000); // 300ms没新数据就停止

        DispatcherTimer _uiTimer = new DispatcherTimer();

        private readonly object _lock = new object();

        private volatile bool _hasNewData = false;
        public ProtocolDebuggingToolDialog()
        {
            InitializeComponent();
            Loaded += OnLoaded;
            Instance = this;

            _uiTimer.Interval = TimeSpan.FromMilliseconds(1000);
            _uiTimer.Tick += UiTimer_Tick;
            _uiTimer.Start();

            if (App.permissionMode != PermissionMode.NoPassword)
            {
                txtHexInput.Visibility = Visibility.Visible;
                txtHexInputButton.Visibility = Visibility.Visible;
            }
            else
            {
                txtHexInput.Visibility = Visibility.Collapsed;
                txtHexInputButton.Visibility = Visibility.Collapsed;
            }
        }

        private void UiTimer_Tick(object sender, EventArgs e)
        {
            if (!_hasNewData)
            {
                _dataFrequencyMonitor.CheckSilentClear();
                hztext.Text = _dataFrequencyMonitor.LastSecondCount.ToString();
                return;
            }


            var now = DateTime.Now;
            string currentData;
            byte[] currentDataBuf = null;
            lock (_lock)
            {
                currentData = CurrentData;
                currentDataBuf = CurrentDataBytes;
            }

            AppendReceivedData(currentData);
            AppendReceivedData(CurrentDataBytes, Cmd, Id);
            _hasNewData = false;
        }

        private void OnLoaded(object sender, RoutedEventArgs e)
        {
            _receivedDataHistory = new List<string>();
            _idRadioButtons = new List<RadioButton>();
            InitializeProtocols();
            UpdateIdPanel();
        }

        private void InitializeProtocols()
        {
            _allCommands = new ObservableCollection<ProtocolCommand>
            {
                new ProtocolCommand
                {
                    Code = (byte)UserCommlmpl.Cmd.Cmd0,
                    Name = "基础设置",
                    Description = "查询指定ID的数据",
                    HexFormat = "AA 55 00 [ID] XX",
                    AvailableIds = new List<ProtocolId>
                    {
                        new ProtocolId { Code = (byte)UserCommlmpl.Id.Id1, Name = "传感器数据推送" },
                        new ProtocolId { Code = (byte)UserCommlmpl.Id.Id2, Name = "设备状态数据推送" },
                        new ProtocolId { Code = (byte)UserCommlmpl.Id.Id3, Name = "版本查询" },
                        new ProtocolId { Code = (byte)UserCommlmpl.Id.Id4, Name = "传感器数据推送频率设置与查询" },
                        new ProtocolId { Code = (byte)UserCommlmpl.Id.Id5, Name = "修改蓝牙名称" },
                        new ProtocolId { Code = (byte)UserCommlmpl.Id.Id6, Name = "算法模式查询与设置" },
                        new ProtocolId { Code = (byte)UserCommlmpl.Id.Id7, Name = "用户数据推送配置与查询" },
                        new ProtocolId { Code = (byte)UserCommlmpl.Id.Id8, Name = "地磁开关" },
                        new ProtocolId { Code = (byte)UserCommlmpl.Id.Id9, Name = "地磁数据解析" },
                        new ProtocolId { Code =(byte)UserCommlmpl.Id.Id10, Name = "传感器推送数据2" }
                    }
                },
                new ProtocolCommand
                {
                    Code = (byte)UserCommlmpl.Cmd.Cmd1,
                    Name = "其他数据获取",
                    Description = "查询指定ID的数据",
                    HexFormat = "AA 55 01 [ID] XX",
                    AvailableIds = new List<ProtocolId>
                    {
                        new ProtocolId { Code = (byte)UserCommlmpl.Id.Id1, Name = "A值查询与设置" },
                        new ProtocolId { Code = (byte)UserCommlmpl.Id.Id2, Name = "设备状态与传感器数据" },
                        new ProtocolId { Code = (byte)UserCommlmpl.Id.Id3, Name = "电池电量" }
                    }
                },
                //new ProtocolCommand
                //{
                //    Code = 2,
                //    Name = "数据调试",
                //    Description = "设置指定ID的参数",
                //    HexFormat = "AA 02 [ID] [值] XX",
                //    AvailableIds = new List<ProtocolId>
                //    {
                //        new ProtocolId { Code = 1, Name = "功能1" },
                //        new ProtocolId { Code = 2, Name = "功能2" },
                //        new ProtocolId { Code = 3, Name = "功能3" }
                //    }
                //}
            };

            _filteredCommands = new ObservableCollection<ProtocolCommand>(_allCommands);
            CmdListBox.ItemsSource = _filteredCommands;
            CmdListBox.SelectedIndex = 0;
        }

        private void UpdateIdPanel()
        {
            IdPanel.Children.Clear();
            _idRadioButtons.Clear();

            var header = new TextBlock
            {
                Text = "选择ID（单选）:",
                FontWeight = FontWeights.SemiBold,
                Margin = new Thickness(0, 0, 0, 8),
                FontSize = 12,
                Foreground = new SolidColorBrush((Color)ColorConverter.ConvertFromString("#2C3E50"))
            };
            IdPanel.Children.Add(header);

            if (_currentCmd == null || _currentCmd.AvailableIds.Count == 0)
            {
                var noIdHint = new TextBlock
                {
                    Text = "（此CMD无需选择ID）",
                    FontSize = 11,
                    Foreground = new SolidColorBrush((Color)ColorConverter.ConvertFromString("#7F8C8D"))
                };
                IdPanel.Children.Add(noIdHint);
                return;
            }

            foreach (var id in _currentCmd.AvailableIds)
            {
                var radioButton = new RadioButton
                {
                    Content = $"[ 0x{id.Code:X2}] {id.Name}",
                    Tag = id,
                    Margin = new Thickness(0, 4, 0, 0),
                    FontSize = 12,
                    GroupName = "IdGroup"
                };
                radioButton.HorizontalAlignment = HorizontalAlignment.Left;
                radioButton.Checked += IdRadioButton_Changed;

                var toolTip = new ToolTip
                {
                    Content = $"ID代码: {id.Code:X2}"
                };
                radioButton.ToolTip = toolTip;

                _idRadioButtons.Add(radioButton);
                IdPanel.Children.Add(radioButton);
            }
        }

        private void CmdSearchBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            var searchText = CmdSearchBox.Text.ToLower().Trim();

            if (string.IsNullOrEmpty(searchText))
            {
                _filteredCommands = new ObservableCollection<ProtocolCommand>(_allCommands);
            }
            else
            {
                var filtered = _allCommands
                    .Where(p => p.Name.ToLower().Contains(searchText) ||
                               p.Code.ToString().Contains(searchText) ||
                               p.Description.ToLower().Contains(searchText))
                    .ToList();
                _filteredCommands = new ObservableCollection<ProtocolCommand>(filtered);
            }

            CmdListBox.ItemsSource = _filteredCommands;
        }

        private void CmdListBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (CmdListBox.SelectedItem is ProtocolCommand selected)
            {
                _currentCmd = selected;
                UpdateCmdDetails(selected);
                UpdateIdPanel();
            }
        }

        private void UpdateCmdDetails(ProtocolCommand cmd)
        {
            CmdNameText.Text = cmd.Name;
            HexFormatText.Text = cmd.HexFormat;
            GeneratedHexText.Text = string.Empty;
            StatusText.Text = $"已选择CMD: [{cmd.Code}] {cmd.Name}";
            Cmd = cmd.Code;
            StatusText.Foreground = new SolidColorBrush((Color)ColorConverter.ConvertFromString("#3498DB"));
        }

        private void IdRadioButton_Changed(object sender, RoutedEventArgs e)
        {
            if (sender is RadioButton rb && rb.IsChecked == true && rb.Tag is ProtocolId id)
            {
                StatusText.Text = $"已选择ID: [{id.Code}] {id.Name}";
                Id = id.Code;
                StatusText.Foreground = new SolidColorBrush((Color)ColorConverter.ConvertFromString("#27AE60"));
            }
        }

        private void GenerateHexButton_Click(object sender, RoutedEventArgs e)
        {
            if (_currentCmd == null)
            {
                StatusText.Text = "请先选择CMD命令";
                StatusText.Foreground = new SolidColorBrush((Color)ColorConverter.ConvertFromString("#F39C12"));
                return;
            }

            var selectedId = _idRadioButtons
                .FirstOrDefault(rb => rb.IsChecked == true)?
                .Tag as ProtocolId;

            var hex = GenerateHexForCommand(_currentCmd, selectedId);
            GeneratedHexText.Text = hex;
            StatusText.Text = "HEX生成成功";
            StatusText.Foreground = new SolidColorBrush((Color)ColorConverter.ConvertFromString("#27AE60"));
        }

        private string GenerateHexForCommand(ProtocolCommand cmd, ProtocolId selectedId)
        {
            var sb = new StringBuilder();



            sb.AppendLine("=== HEX指令生成 ===");
            sb.AppendLine($"CMD: [{Cmd}] ID:[{Id}]");
            byte[] data = AppendReceivedData(Cmd, Id);
            byte[] bytes = GetUserInfo(data, Cmd, Id, 0);
            if (byte.TryParse(OpInput.Text.Trim(), out byte value))
            {
                bytes = GetUserInfo(data, Cmd, Id, value);
            }
            string receivedString = BitConverter.ToString(bytes).Replace("-", " ");
            sb.AppendLine($"内容: {receivedString}");

            return sb.ToString();
        }

        private void SendCommandButton_Click(object sender, RoutedEventArgs e)
        {
            if (!string.IsNullOrEmpty(GeneratedHexText.Text))
            {
                StatusText.Text = "指令发送中...";
                StatusText.Foreground = new SolidColorBrush((Color)ColorConverter.ConvertFromString("#3498DB"));
                byte[] data = AppendReceivedData(Cmd, Id);
                byte[] bytes = GetUserInfo(data, Cmd, Id, 0);
                if (byte.TryParse(OpInput.Text.Trim(), out byte value))
                {
                    bytes = SendUserInfo(data, Cmd, Id, value);
                }

                string receivedString = BitConverter.ToString(bytes).Replace("-", " ");
                AppendReceivedData($"[TX] {DateTime.Now:HH:mm:ss.fff} - CMD:{_currentCmd?.Code} - Data:{receivedString}");
            }
            else
            {
                StatusText.Text = "请先生成HEX";
                StatusText.Foreground = new SolidColorBrush((Color)ColorConverter.ConvertFromString("#F39C12"));
            }
        }

        private string CalculateChecksum(string data)
        {
            var bytes = data.Split(' ')
                .Where(s => !string.IsNullOrEmpty(s))
                .Select(s => Convert.ToByte(s, 16))
                .ToArray();

            byte sum = 0;
            foreach (var b in bytes)
            {
                sum += b;
            }

            return sum.ToString("X2");
        }

        private void CopyHexButton_Click(object sender, RoutedEventArgs e)
        {
            if (!string.IsNullOrEmpty(GeneratedHexText.Text))
            {
                var hexOnly = ExtractHexLine(GeneratedHexText.Text);
                if (!string.IsNullOrEmpty(hexOnly))
                {
                    Clipboard.SetText(hexOnly);
                    StatusText.Text = "HEX已复制到剪贴板";
                    StatusText.Foreground = new SolidColorBrush((Color)ColorConverter.ConvertFromString("#27AE60"));
                }
            }
        }

        private string ExtractHexLine(string text)
        {
            var lines = text.Split('\n');
            foreach (var line in lines)
            {
                if (line.Trim().StartsWith("AA"))
                {
                    return line.Trim();
                }
            }
            return text;
        }



        private void ClearDataButton_Click(object sender, RoutedEventArgs e)
        {
            ReceivedDataText.Text = string.Empty;
            _receivedDataHistory.Clear();
        }

        public void AppendReceivedData(string data)
        {
            _receivedDataHistory.Add(data);

            if (_receivedDataHistory.Count > MaxHistoryCount)
            {
                _receivedDataHistory.RemoveAt(0);
            }

            var sb = new StringBuilder();
            foreach (var item in _receivedDataHistory)
            {
                sb.AppendLine(item);
            }
            ReceivedDataText.Text = sb.ToString();
            ReceivedDataText.ScrollToEnd();
        }

        public byte[] AppendReceivedData(byte cmd = 0, byte id = 0)
        {
            switch (cmd)
            {
                case (byte)UserCommlmpl.Cmd.Cmd0:
                    return HandleCmd0(cmd, id);
                case (byte)UserCommlmpl.Cmd.Cmd1:
                    return HandleCmd1(cmd, id);
                case (byte)UserCommlmpl.Cmd.Cmd2:
                    return HandleCmd2(cmd, id);
                default:
                    Console.WriteLine($"未处理的命令: {cmd}");
                    return null;
            }
        }

        private byte[] HandleCmd0(byte cmd, byte id)
        {
            switch (id)
            {
                case (byte)UserCommlmpl.Id.Id1:
                    return null;
                case (byte)UserCommlmpl.Id.Id2:
                    return null;
                case (byte)UserCommlmpl.Id.Id3:
                    return null;
                case (byte)UserCommlmpl.Id.Id4:
                    byte[] id4 = new byte[1];
                    id4[0] = 0;
                    if (byte.TryParse(Data1Text.Text.Trim(), out byte value))
                    {
                        id4[0] = value;
                    }
                    return id4;
                case (byte)UserCommlmpl.Id.Id5:
                    byte[] sn = new byte[16];
                    for (int i = 0; i < Data1Text.Text.Length && i < sn.Length; i++)
                    {
                        sn[i] = (byte)Data1Text.Text[i];
                    }
                    return sn;
                case (byte)UserCommlmpl.Id.Id6:
                    byte[] id6 = new byte[1];
                    id6[0] = 0;
                    if (byte.TryParse(Data1Text.Text.Trim(), out byte value1))
                    {
                        id6[0] = value1;
                    }
                    return id6;
                case (byte)UserCommlmpl.Id.Id7:
                    byte[] id7 = new byte[1];
                    id7[0] = 0;
                    if (byte.TryParse(Data1Text.Text.Trim(), out byte value2))
                    {
                        id7[0] = value2;
                    }
                    return id7;
                case (byte)UserCommlmpl.Id.Id8:
                    byte[] id8 = new byte[1];
                    id8[0] = 0;
                    if (byte.TryParse(Data1Text.Text.Trim(), out byte value3))
                    {
                        id8[0] = value3;
                    }
                    return id8;
                case (byte)UserCommlmpl.Id.Id9:

                    return null;
                case (byte)UserCommlmpl.Id.Id10:
                    return null;
                default:
                    return null;
            }
        }

        private byte[] HandleCmd1(byte cmd, byte id)
        {
            switch (id)
            {
                case (byte)UserCommlmpl.Id.Id1:
                    byte[] id1 = new byte[1];
                    id1[0] = 0;
                    if (byte.TryParse(Data1Text.Text.Trim(), out byte value))
                    {
                        id1[0] = value;
                    }
                    return id1;
                case (byte)UserCommlmpl.Id.Id2:
                    return null;
                case (byte)UserCommlmpl.Id.Id3:
                    byte[] id3 = new byte[1];
                    return id3;
                default:
                    return null;
            }
        }

        private byte[] HandleCmd2(byte cmd, byte id)
        {
            switch (id)
            {
                case (byte)UserCommlmpl.Id.Id1:
                    byte[] id1 = new byte[1];
                    return id1;
                case (byte)UserCommlmpl.Id.Id2:
                    return null;
                case (byte)UserCommlmpl.Id.Id3:
                    byte[] id3 = new byte[1];
                    return id3;
                default:
                    return null;
            }
        }

        public void AppendReceivedData(byte[] buf, byte cmd = 0, byte id = 0)
        {
            switch (cmd)
            {
                case (byte)UserCommlmpl.Cmd.Cmd0:
                    HandleCmd0(cmd, id, buf);
                    break;
                case (byte)UserCommlmpl.Cmd.Cmd1:
                    HandleCmd1(cmd, id, buf);
                    break;
                case (byte)UserCommlmpl.Cmd.Cmd2:
                    HandleCmd2(cmd, id, buf);
                    break;
                default:
                    Console.WriteLine($"未处理的命令: {cmd}");
                    break;
            }
        }
        private StringBuilder _logSb = new StringBuilder();
        private void HandleCmd0(byte cmd, byte id, byte[] buf)
        {
            _logSb.Clear();
            _logSb.AppendLine($"--- 收到命令 Cmd={cmd}, Id={id} ---");
            switch (id)
            {
                case (byte)UserCommlmpl.Id.Id1:
                    SensorDataPacket imuData = CommImpl.ByteArrayToStructure<SensorDataPacket>(buf);

                    _logSb.AppendLine("【类型】: IMU 传感器数据");
                    _logSb.AppendLine($"频率: {imuData.PushFrequency:F2} Hz");
                    _logSb.AppendLine($"时间: {imuData.Year}-{imuData.Month:D2}-{imuData.Day:D2} {imuData.Hour:D2}:{imuData.Minute:D2}:{imuData.Second:D2}");
                    _logSb.AppendLine($"加速度: [{imuData.Acceleration[0]:F3}, {imuData.Acceleration[1]:F3}, {imuData.Acceleration[2]:F3} ,{imuData.Acceleration[3]:F3}] m/s²");
                    _logSb.AppendLine($"角速度: [{imuData.AngularVelocity[0]:F3}, {imuData.AngularVelocity[1]:F3}, {imuData.AngularVelocity[2]:F3} , {imuData.AngularVelocity[3]:F3} ] deg/s");
                    _logSb.AppendLine($"温度:   {imuData.Temperature:F2} °C");
                    _logSb.AppendLine($"姿态角: [{imuData.EulerAngles[0]:F2}, {imuData.EulerAngles[1]:F2}, {imuData.EulerAngles[2]:F2}] deg");
                    _logSb.AppendLine($"四元数: [{imuData.Quaternion[0]:F4}, {imuData.Quaternion[1]:F4}, {imuData.Quaternion[2]:F4}, {imuData.Quaternion[3]:F4}]");
                    _logSb.AppendLine($"磁场:   [{imuData.Magnetometer[0]:F2}, {imuData.Magnetometer[1]:F2}, {imuData.Magnetometer[2]:F2}] uT");
                    _logSb.AppendLine($"CRC16:  0x{imuData.Crc16:X4}");
                    InfoTextBlock.Text = _logSb.ToString();
                    break;
                case (byte)UserCommlmpl.Id.Id2:
                    StatusPacket statusPacket = CommImpl.ByteArrayToStructure<StatusPacket>(buf);
                    _logSb.AppendLine("【类型】: 设备状态信息");
                    _logSb.AppendLine($"用户A值: {statusPacket.UserA}");
                    _logSb.AppendLine($"电量: {statusPacket.BatteryLevel}");
                    _logSb.AppendLine($"CRC16:    0x{statusPacket.Crc16:X4}");
                    InfoTextBlock.Text = _logSb.ToString();
                    break;
                case (byte)UserCommlmpl.Id.Id3:
                    VisonPacket visonPacket = CommImpl.ByteArrayToStructure<VisonPacket>(buf);
                    _logSb.AppendLine("【类型】: 版本信息");
                    _logSb.AppendLine($"版本号: {CommImpl.Transform_TestAckAPPandLoader(visonPacket.vison)}");
                    _logSb.AppendLine($"CRC16:    0x{visonPacket.Crc16:X4}");
                    InfoTextBlock.Text = _logSb.ToString();
                    break;
                case (byte)UserCommlmpl.Id.Id4:
                    HzPacket hzPacket = CommImpl.ByteArrayToStructure<HzPacket>(buf);
                    _logSb.AppendLine("【类型】: 数据推送频率信息");
                    _logSb.AppendLine($"Hz: {hzPacket.hz}");
                    _logSb.AppendLine($"CRC16:    0x{hzPacket.Crc16:X4}");
                    InfoTextBlock.Text = _logSb.ToString();
                    break;
                case (byte)UserCommlmpl.Id.Id5:
                    SnPacket snPacket = CommImpl.ByteArrayToStructure<SnPacket>(buf);
                    _logSb.AppendLine("【类型】: SN设置状态信息");
                    _logSb.AppendLine($"占位数据: {snPacket.data}");
                    _logSb.AppendLine($"CRC16:    0x{snPacket.Crc16:X4}");
                    InfoTextBlock.Text = _logSb.ToString();
                    break;
                case (byte)UserCommlmpl.Id.Id6:
                    AlgorithmPacket algorithmPacket = CommImpl.ByteArrayToStructure<AlgorithmPacket>(buf);
                    _logSb.AppendLine("【类型】: 算法信息");
                    _logSb.AppendLine($"算法模式: {algorithmPacket.algorithm}");
                    _logSb.AppendLine($"CRC16:    0x{algorithmPacket.Crc16:X4}");
                    InfoTextBlock.Text = _logSb.ToString();
                    break;
                case (byte)UserCommlmpl.Id.Id7:
                    APacket aPacket = CommImpl.ByteArrayToStructure<APacket>(buf);
                    _logSb.AppendLine("【类型】: A值信息");
                    _logSb.AppendLine($"A值状态: {aPacket.a}");
                    _logSb.AppendLine($"CRC16:    0x{aPacket.Crc16:X4}");
                    InfoTextBlock.Text = _logSb.ToString();
                    break;
                case (byte)UserCommlmpl.Id.Id8:
                    APacket aPacket1 = CommImpl.ByteArrayToStructure<APacket>(buf);
                    _logSb.AppendLine("【类型】: A值信息");
                    _logSb.AppendLine($"开关状态值: {aPacket1.a}");
                    _logSb.AppendLine($"CRC16:    0x{aPacket1.Crc16:X4}");
                    InfoTextBlock.Text = _logSb.ToString();
                    break;
                case (byte)UserCommlmpl.Id.Id9:
                    CalibrationData calibrationData = CommImpl.ByteArrayToStructure<CalibrationData>(buf);
                    _logSb.AppendLine("【类型】: 校准数据");
                    _logSb.AppendLine($"校准进度: {calibrationData.Progress:F2}");
                    _logSb.AppendLine($"校准是否有效: {calibrationData.IsValid}");
                    _logSb.AppendLine($"涵盖极值方向数量: {calibrationData.DirCount}");
                    _logSb.AppendLine($"有效样本数量: {calibrationData.SampleCount}");
                    _logSb.AppendLine($"CRC16:    0x{calibrationData.Crc16:X4}");
                    InfoTextBlock.Text = _logSb.ToString();
                    break;
                case (byte)UserCommlmpl.Id.Id10:
                    // 字节数组转 传感器数据结构体
                    SensorData sensorData = CommImpl.ByteArrayToStructure<SensorData>(buf);

                    _logSb.AppendLine("【类型】: 传感器推送数据 (0x0010)");
                    _logSb.AppendLine($"时间戳(us): {sensorData.Timestamp}");

                    _logSb.AppendLine($"加速度 X: {sensorData.Acc[0]:F2} m/s²");
                    _logSb.AppendLine($"加速度 Y: {sensorData.Acc[1]:F2} m/s²");
                    _logSb.AppendLine($"加速度 Z: {sensorData.Acc[2]:F2} m/s²");

                    _logSb.AppendLine($"角速度 X: {sensorData.Gyro[0]:F2} °/S");
                    _logSb.AppendLine($"角速度 Y: {sensorData.Gyro[1]:F2} °/S");
                    _logSb.AppendLine($"角速度 Z: {sensorData.Gyro[2]:F2} °/S");

                    _logSb.AppendLine($"姿态 Roll: {sensorData.Angle[0]:F2} °");
                    _logSb.AppendLine($"姿态 Pitch: {sensorData.Angle[1]:F2} °");
                    _logSb.AppendLine($"姿态 Yaw:   {sensorData.Angle[2]:F2} °");

                    _logSb.AppendLine($"CRC16: 0x{sensorData.Crc16:X4}");

                    InfoTextBlock.Text = _logSb.ToString();
                    break;
                default:
                    break;
            }
        }

        private void HandleCmd1(byte cmd, byte id, byte[] buf)
        {
            _logSb.Clear();
            _logSb.AppendLine($"--- 收到命令 Cmd={cmd}, Id={id} ---");
            switch (id)
            {
                case (byte)UserCommlmpl.Id.Id1:
                    APacket aPacket = CommImpl.ByteArrayToStructure<APacket>(buf);
                    _logSb.AppendLine("【类型】: A值信息");
                    _logSb.AppendLine($"用户A值: {aPacket.a}");
                    _logSb.AppendLine($"CRC16:    0x{aPacket.Crc16:X4}");
                    InfoTextBlock.Text = _logSb.ToString();
                    break;
                case (byte)UserCommlmpl.Id.Id2:
                    AllDataPacket imuData = CommImpl.ByteArrayToStructure<AllDataPacket>(buf);

                    _logSb.AppendLine("【类型】: IMU 传感器数据");
                    _logSb.AppendLine($"频率: {imuData.PushFrequency:F2} Hz");
                    _logSb.AppendLine($"加速度: [{imuData.Acceleration[0]:F3}, {imuData.Acceleration[1]:F3}, {imuData.Acceleration[2]:F3} ,{imuData.Acceleration[3]:F3}] m/s²");
                    _logSb.AppendLine($"角速度: [{imuData.AngularVelocity[0]:F3}, {imuData.AngularVelocity[1]:F3}, {imuData.AngularVelocity[2]:F3} , {imuData.AngularVelocity[3]:F3} ] deg/s");
                    _logSb.AppendLine($"温度:   {imuData.Temperature:F2} °C");
                    _logSb.AppendLine($"姿态角: [{imuData.EulerAngles[0]:F2}, {imuData.EulerAngles[1]:F2}, {imuData.EulerAngles[2]:F2}] deg");
                    _logSb.AppendLine($"四元数: [{imuData.Quaternion[0]:F4}, {imuData.Quaternion[1]:F4}, {imuData.Quaternion[2]:F4}, {imuData.Quaternion[3]:F4}]");
                    _logSb.AppendLine($"磁场:   [{imuData.Magnetometer[0]:F2}, {imuData.Magnetometer[1]:F2}, {imuData.Magnetometer[2]:F2}] uT");
                    _logSb.AppendLine($"用户A值: {imuData.UserA}");
                    _logSb.AppendLine($"电量: {imuData.BatteryLevel}");
                    _logSb.AppendLine($"CRC16:  0x{imuData.Crc16:X4}");
                    InfoTextBlock.Text = _logSb.ToString();
                    break;
                case (byte)UserCommlmpl.Id.Id3:
                    PowerPacket powerPacket = CommImpl.ByteArrayToStructure<PowerPacket>(buf);
                    _logSb.AppendLine("【类型】: A值信息");
                    _logSb.AppendLine($"用户A值: {powerPacket.power}");
                    _logSb.AppendLine($"CRC16:    0x{powerPacket.Crc16:X4}");
                    InfoTextBlock.Text = _logSb.ToString();
                    break;
                default:

                    break;
            }
        }

        private void HandleCmd2(byte cmd, byte id, byte[] buf)
        {
            switch (id)
            {
                case (byte)UserCommlmpl.Id.Id1:

                    break;
                case (byte)UserCommlmpl.Id.Id2:

                    break;
                case (byte)UserCommlmpl.Id.Id3:

                    break;
                default:

                    break;
            }
        }



        private static readonly FrequencyMonitor _dataFrequencyMonitor = new FrequencyMonitor();
        public int AppendReceivedDataFrequency(byte cmd = 0, byte id = 0)
        {
            if (Cmd == cmd && Id == id)
            {
                // 【频率统计】每进来一次统计一次，线程安全
                _dataFrequencyMonitor.Increment();

                // 优化：减少频繁Invoke造成卡顿，直接传递频率数值
                int nowHz = _dataFrequencyMonitor.LastSecondCount;
                // 空数据判断
                return nowHz;
            }
            return 0;
        }
        public void AppendReceivedData(byte[] buf, byte cmd = 0, byte id = 0, string data = "", int hz = 0)
        {
            if (Cmd == cmd && Id == id)
            {
                _lastDataReceiveTime = DateTime.Now;
                CurrentData = data;
                CurrentDataBytes = buf;
                _hasNewData = true;
                hztext.Text = hz.ToString();
            }
        }

        private void ShowHexCodeButton_Click(object sender, RoutedEventArgs e)
        {
            var codeWindow = new HexCodeDisplayWindow();
            codeWindow.Owner = this;
            codeWindow.ShowDialog();
        }

        private void ShowParsingLogicButton_Click(object sender, RoutedEventArgs e)
        {
            var parsingWindow = new HexParsingLogicWindow();
            parsingWindow.Owner = this;
            parsingWindow.ShowDialog();
        }

        /// <summary>
        /// HEX空格字符串转byte数组 例:"A5 04 00"
        /// </summary>
        public static byte[] HexToBytes(string hexStr)
        {
            if (string.IsNullOrWhiteSpace(hexStr)) return new byte[0];
            hexStr = hexStr.Replace(" ", "").ToUpper();
            hexStr = hexStr.Replace("0x", "").ToUpper();
            hexStr = hexStr.Replace("0X", "").ToUpper();
            if (hexStr.Length % 2 != 0) return new byte[0];

            byte[] data = new byte[hexStr.Length / 2];
            for (int i = 0; i < data.Length; i++)
            {
                data[i] = Convert.ToByte(hexStr.Substring(i * 2, 2), 16);
            }
            return data;
        }

        private void BtnSendHex_Click(object sender, RoutedEventArgs e)
        {
            txtHexInputTip.Text = "";
            // 取输入框HEX
            string hexTxt = txtHexInput.Text.Trim();
            // 转字节数组
            byte[] dataPacket = HexToBytes(hexTxt);

            // 你原来的入队逻辑
            if (dataPacket != null && dataPacket.Length > 0)
            {
                txtHexInputTip.Text= BspHexParse(dataPacket);
            }
        }

        // ================== 常量 ==================
        private const int HEX_MAX_PACKET_LENGTH = 256;

        // ================== 外部依赖（你项目里已有） ==================
        // ushort CRC.CRC16(byte[] data, ushort len)
        // void UserCommlmpl.user_unpack_data_fun(byte[] data, ushort len, int a, int b)

        /// <summary>
        /// 直接传入完整 byte[] 数据包解析（从16进制字符串转来的 byte[]）
        /// </summary>
        /// <param name="dataPacket">完整数据包字节数组</param>
        /// <returns>返回错误信息，成功返回空或成功提示</returns>
        public string BspHexParse(byte[] dataPacket)
        {
            // 1. 空包判断
            if (dataPacket == null || dataPacket.Length < 6)
            {
                return "错误：数据包为空或长度不足";
            }

            // 2. 校验帧头 0xAA 0x55
            if (dataPacket[0] != 0xAA || dataPacket[1] != 0x55)
            {
                return "错误：帧头不正确，非 0xAA 0x55 开头";
            }

            // 3. 读取包长度（完全保留你原来的计算逻辑）
            ushort hexPacketLength = (ushort)(
                dataPacket[4]                  // 低字节（第5个字节，索引4）
                + ((dataPacket[5] & 0x0F) << 8) // 高4位（第6个字节，索引5）
            );

            // 4. 长度合法性校验
            if (hexPacketLength < 6 || hexPacketLength > HEX_MAX_PACKET_LENGTH || hexPacketLength > dataPacket.Length)
            {
                return "错误：数据包长度不合法或超出最大限制";
            }

            // 5. 提取CRC（最后两个字节，高字节在前）
            ushort receivedCrc = (ushort)(
                (dataPacket[hexPacketLength - 1] << 8)
                | dataPacket[hexPacketLength - 2]
            );

            // 6. CRC校验（校验：除最后2字节CRC外的所有数据）
            ushort calcCrc = CRC.CRC16(dataPacket, (ushort)(hexPacketLength - 2));
            if (calcCrc != receivedCrc)
            {
                return "错误：CRC校验失败";
            }

            // 成功返回空 或 "成功"
            return "验证通过";
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            _uiTimer.Tick -= UiTimer_Tick;
            Instance = null;
            Loaded -= OnLoaded;
        }

        private void txtHexInput_TextChanged(object sender, TextChangedEventArgs e)
        {
            txtHexInputTip.Text = "";
        }
    }

    public class ProtocolCommand
    {
        public byte Code { get; set; }
        public string Name { get; set; }
        public string Description { get; set; }
        public string HexFormat { get; set; }
        public List<ProtocolId> AvailableIds { get; set; } = new List<ProtocolId>();
    }

    public class ProtocolId
    {
        public byte Code { get; set; }
        public string Name { get; set; }
    }
}
